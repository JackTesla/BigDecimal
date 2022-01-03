//
// Created by VULAN on 2021/12/3.
//

#include <iostream>
#include <string>

using namespace std;
class BigDecimal {
    // data type : left[0...511].left[512...2047]
private:
    int left[2560], leftpos=0, rightpos=0, MAX_SIZE=2560; ; // 限定小数有效位数
public:
    bool positive=true;
    BigDecimal(double a=0) {
        if(a<0) {  a=-a;   positive=false; }
        int i=0; // 数组下标i
        long long num=(long long) a;
        a-=num;  // 小数
        for(i=0;i<MAX_SIZE;++i) left[i]=0;
        i=511;
        while(num>0) {
            left[i]=num%10;
            int tmp=left[i];
            num=num/10;
            --i;
        }
        // 小数部分
        i=512;
        while(a>0 && i<MAX_SIZE) {
            left[i]= (int)(a*10);
            a=a*10-left[i];
            ++i;
        }
        length();
    }

    BigDecimal(string a) {
        int len=a.length();
        int ipos=len;
        for(int i=0;i<2560;++i) {
            if(a[i]=='.') {
                ipos=i;
            }
        }
        for(int i=ipos-1;i>=0;--i) {
            left[i]=a[i]-'0';
        }
        for(int i=ipos+1;i<len;++i) {
            left[i]=a[i]-'0';
        }
    }

    BigDecimal operator++(){
        this->left[511]++;
        for(int i=511;i>0;--i) {
            if(left[i]>9) {
                left[i-1]+=left[i]%10;
                left[i]=left[i]/10;
            }
        }
        return *this;
    }

    BigDecimal operator+(BigDecimal &b) {
        BigDecimal ret;
        int i=0;
        int maxlen=b.rightpos, minleftpos=b.leftpos;
        if(rightpos > maxlen) maxlen=rightpos;
        if(leftpos<minleftpos) minleftpos=leftpos;
        --minleftpos;
        for(i=maxlen;i>=minleftpos;--i) {
            ret.left[i]+=this->left[i]+b.left[i];
            if(ret.left[i]>9) {
                if(i>0) {
                    ret.left[i - 1]+=ret.left[i]/10;
                    ret.left[i]=ret.left[i]%10;
                }
            }
        }
        ret.length();
        return ret;
    }

    BigDecimal operator+(double a) {
        BigDecimal b(a);
        return b.add(*this,b);
    }

    BigDecimal operator-(double a) {
        BigDecimal ba(a);
        return sub(*this,ba);
    }

    BigDecimal operator-(BigDecimal a) {
        return sub(*this,a);
    }

    BigDecimal operator*(double a) {
        BigDecimal b(a);
        return this->mul(*this,b);
    }

    BigDecimal operator*(BigDecimal a){
        return this->mul(*this,a);
    }

    BigDecimal operator/(double a) {
        BigDecimal ba(a);
        return div(*this,ba);
    }

    BigDecimal operator/(BigDecimal a) {
        return div(*this,a);
    }

    BigDecimal add(BigDecimal a, BigDecimal b) {
        BigDecimal ret; // 考虑了负数
        int i=0;
        if(a.positive && b.positive==false) {
            b.positive=true;
            ret=sub(a,b);
            b.positive=false;
            return ret;
        }
        if(a.positive==false && b.positive==true) {
            a.positive=true;
            ret=sub(b,a);
            a.positive=false;
            return ret;
        }
        if(a.positive==false && b.positive==false) ret.positive=false;
        int maxRight=b.rightpos, minLeft=b.leftpos;
        if(a.rightpos > maxRight) maxRight=a.rightpos;
        if(a.leftpos<minLeft) minLeft=a.leftpos;
        if(minLeft > 0) --minLeft;
        for(i=maxRight;i>=minLeft;--i) {
            ret.left[i]+=a.left[i]+b.left[i];
            if(ret.left[i]>9) {
                if(i>0) {
                    ret.left[i - 1]+=ret.left[i]/10;
                    ret.left[i]=ret.left[i]%10;
                }
            }
        }
        ret.length();
        return ret;
    }

    BigDecimal matrixAdd(int a[]) {
        for(int i=MAX_SIZE-1;i>=0;--i) {
            left[i]+=a[i];
            if(left[i]>9) {
                if(i>0) {
                    left[i-1]+=left[i]/10;
                    left[i]=left[i]%10;
                }
            }
        }
        length();
        return *this;
    }

    BigDecimal sub(BigDecimal a ,BigDecimal b) {
        BigDecimal ret;   // 考虑了负数
        string sa=a.toString2(),sb=b.toString2(),sr;
        if(a.positive==false && b.positive) {
            ret=a;
            ret.positive=true;
            ret=ret+b;
            ret.positive=false;
        }
        else if(a.positive==false && b.positive==false) {
            a.positive=true;
            b.positive=true;
            if(compare2(a,b)) { //比较a b的绝对值
                ret=sub(a,b);
                ret.positive=false;
            } else {
                ret=sub(b,a);
            }
            a.positive=false;
            b.positive=false;
        }
        else if(a.positive && b.positive==false) {
            b.positive=true;
            ret=add(a,b);
            b.positive=false;
        }
        else if(a.positive && b.positive) {
            if(compare2(a,b)) {
                int maxRight=a.rightpos>b.rightpos?a.rightpos:b.rightpos;
                int minLeft=a.leftpos;
                if(minLeft>0) --minLeft;
                for(;maxRight>=minLeft;--maxRight) {
                    if(a.left[maxRight]<b.left[maxRight]) {
                        a.left[maxRight-1]--;
                        ret.left[maxRight]+=a.left[maxRight]+10-b.left[maxRight];
                    }else ret.left[maxRight]+=a.left[maxRight]-b.left[maxRight];
                }
            } else {
                ret=sub(b,a);
                ret.positive=false;
            }
        }
        ret.length();
        return ret;
    }

    BigDecimal div(double na,double nb) {
        // 不考虑负数的情况
        BigDecimal a(na), b(nb);
        return div(a,b);
    }

    BigDecimal div(BigDecimal a,BigDecimal &b) {
        // 不考虑负数的情况, d 是b的段长, j 是商的第一位位置
        BigDecimal ret;
        if(b.isZero()) return ret;
        a.getpos(); b.getpos();    //  对于0.000123 的情况更新 pos
        int d=b.rightpos-b.leftpos+1;
        if(d>(MAX_SIZE>>1)) d=(MAX_SIZE>>1);
        int j=511-((512-a.leftpos)-(512-b.leftpos)); // a与b同段长,j的位置
        int starta=0, enda=0;
        starta=a.leftpos;
        enda=starta+d-1; // a与b同段长比较, enda的位置不一定是rightpos
        for(;j<2000 && a.leftpos<= a.rightpos;) {
            if(enda>=MAX_SIZE) enda=MAX_SIZE-1;
            if(!compare(a,b,starta,enda,b.leftpos,b.rightpos)) {
                ++j; // 考虑a与b同段长的片段，如果a<b，那么商的位置j后移一位，同时++enda
                ++enda;
                if(enda>=MAX_SIZE) enda=MAX_SIZE-1;
            }
            //sa=a.toString2();
            int ia=enda, ib=b.rightpos;  // 进入while前，由getpos更新的starta位置的值一定不为0
            while(compare(a,b,starta,enda,b.leftpos,b.rightpos)) {
                // 计算单个j位的商，减到a片段小于b为止
                //sa=a.toString2();
                for (ia=enda,ib=b.rightpos ; ia >= starta; --ia, --ib) {
                    if (a.left[ia] < b.left[ib]) { // 如果ia位置的数<ib位置的数,向前借位，相减
                        a.left[ia-1]--;
                        a.left[ia] = a.left[ia] + 10 - b.left[ib];
                    } else a.left[ia] -= b.left[ib];
                }
                //sa=a.toString2();
                ++ret.left[j];
                //sb=ret.toString2();
                //情况1：a片段=0123，b=234，如果starta位置的值为0，starta后移，j位置暂时不变,
                //情况2：a片段与b长度相同，starta位置为0，starta位后移，则下次循环退出,j位置要变
                if (a.left[starta] == 0) ++starta;
            }
            if(a.isZero()) break;
            while(a.left[starta]==0) ++starta;
            // enda与j的位置实际相同,compare同时更新j与enda;
            while(!compare(a,b,starta,enda,b.leftpos,b.rightpos)) {
                ++j; // 考虑a与b同段长的片段，如果a<b，那么商的位置j后移一位，同时++enda
                ++enda;
                if(enda>=MAX_SIZE) enda=MAX_SIZE-1;
            }
            a.getpos();   // 退出while循环后，一定更新j的位置, 更新a.leftpos与endpos,
        }
        ret.length();
        return ret;
    }

    BigDecimal mul(BigDecimal &a, BigDecimal &b) {
        // 不考虑负数
        BigDecimal ret;
        int k=b.rightpos-511+a.rightpos;  // 最后一位积的位置，小数点后有几位加几，最后加上511
        for(int i=b.rightpos,m=0;i>=b.leftpos;--i,++m) { //从b的最后一位开始，依次乘a的每一位，存于ret
            k=b.rightpos-511+a.rightpos-m;  // b每一位乘完一轮，m+1,
            for(int j=a.rightpos;j>=a.leftpos;--j,--k) {
                ret.left[k]+=a.left[j]*b.left[i];
                if(ret.left[k]>9) {
                    ret.left[k-1]+=ret.left[k]/10;
                    ret.left[k]=ret.left[k]%10;
                }
            }
        }
        ret.length();
        if(ret.left[ret.leftpos]>9) {
            ret.left[leftpos-1]=ret.left[ret.leftpos]/10;
            ret.left[ret.leftpos]=ret.left[ret.leftpos]%10;
            ret.length();
        }
        return ret;
    }

    BigDecimal pow(double x, long long k) {
        // 只计算非负数的整数次幂
        BigDecimal ret(1);
        BigDecimal a(x);
        if(k==0) return ret;
        for(long long  i=0;i<k;++i) {
            ret=ret.mul(ret,a);
        }
        return ret;
    }

    BigDecimal pow(BigDecimal &a, BigDecimal &k) {
        // 只计算非负数的整数次幂
        BigDecimal ret(1);
        BigDecimal i(0);
        if(k.isZero()) return ret;
        for(;i<k; i=i+1) {
            ret=ret.mul(ret,a);
        }
        return ret;
    }

    string toString2() {
        int i=0;
        string s="";
        int starti;
        for(starti=0;starti<511;++starti) if(left[starti] !=0) break;
        for(i=starti;i<512;++i) {
            s.append(to_string(left[i])); }
        int endi;
        for(endi=1536;endi>511;--endi) if(left[endi] != 0) break;
        if(endi==511)  return s;
        s.append(".");
        for(i=512;i<=endi;++i) s.append(to_string(left[i]));
        return s;
    }

    void length() {
        int i;
        for(i=0;i<511;++i) if(this->left[i]>0) break;
        this->leftpos=i;
        for(i=MAX_SIZE-1;i>511;--i) if(this->left[i]>0) break;
        this->rightpos=i;
    }

    bool isZero() {
        for(int i=0;i<MAX_SIZE;++i) {
            if (left[i] > 0) return false;
        }
        return true;
    }

    void getpos() {
        int i;
        for(i=0;i<MAX_SIZE;++i) if(this->left[i]>0) break;
        this->leftpos=i;
        for(i=MAX_SIZE-1;i>0;--i) if(this->left[i]>0) break;
        this->rightpos=i;
    }

    bool compare(BigDecimal &a, BigDecimal &b, int starta=0, int enda=1999, int startb=0, int endb=1999) {
        if(enda>=MAX_SIZE) enda=MAX_SIZE-1;
        if(enda-starta > endb-startb) {
            // 1234:123的情况与 0123:234的情况
            if(a.left[starta] != 0) return true;
            starta++;
        }
        if(enda-starta < endb-startb) return false;
        while(a.left[starta]==b.left[startb]) {
            ++starta;
            ++startb;
            if(starta>enda) return true;
        }
        return a.left[starta] >= b.left[startb];
    }

    bool operator<(BigDecimal &b) {
        if(this->leftpos != b.leftpos) return this->leftpos >b.leftpos;
        int maxpos=this->rightpos;
        if(b.rightpos>maxpos) maxpos=b.rightpos;
        int i;
        for(i=b.leftpos;i<=maxpos;++i) {
            if(this->left[i] != b.left[i]) break;
        }
        return this->left[i] < b.left[i];
    }

    bool operator>(BigDecimal &b) {
        if(this->leftpos != b.leftpos) return this->leftpos <b.leftpos;
        int maxpos=this->rightpos;
        if(b.rightpos>maxpos) maxpos=b.rightpos;
        int i;
        for(i=b.leftpos;i<=maxpos;++i) {
            if(this->left[i] != b.left[i]) break;
        }
        return this->left[i] > b.left[i];
    }

    bool compare2(BigDecimal &a , BigDecimal &b){
        if(a.leftpos != b.leftpos) return a.leftpos<b.leftpos;
        int maxpos=b.rightpos;
        if(a.rightpos>maxpos) maxpos=a.rightpos;
        for(int i=a.leftpos;i<=maxpos;++i) {
            if(a.left[i] != b.left[i]) return a.left[i]>b.left[i];
        }
        return false;
    }

    void show2() {
        int i=0;
        string s="";
        if(!this->positive) s.append("-");
        int starti;
        for(starti=0;starti<511;++starti) if(left[starti] !=0) break;
        for(i=starti;i<512;++i) {
            s.append(to_string(left[i])); }
        int endi;
        for(endi=560;endi>511;--endi) if(left[endi] != 0) break;
        if(endi==511) {
            cout<<s<<endl;
            return; }
        s.append(".");
        for(i=512;i<=endi;++i) s.append(to_string(left[i]));
        cout<<s<<endl;
    }

    void show() {
        string s;
        if(!this->positive) s.append("-");
        for(int i=500;i<512;++i) s.append(to_string(left[i]));
        s.append(".");
        for(int i=512;i<560;++i) s.append(to_string(left[i]));
        cout<<s<<endl;
    }

    void showAll() {
        string s="";
        if(!this->positive) s.append("-");
        for(int i=leftpos;i<512;++i) {
            s.append(to_string(left[i])); }
        if(rightpos==511) {
            cout<<s<<endl;
            return; }
        s.append(".");
        for(int i=512;i<=1536;++i) s.append(to_string(left[i]));
        cout<<s<<endl;
    }

    int numAt(int i) {
        cout<<left[i]<<endl;
        return left[i];
    }
};


