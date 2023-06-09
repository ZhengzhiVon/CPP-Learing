#include<iostream>
#include <algorithm>
#include <cmath>
#include<vector>
using namespace std;

//lambda表达式
void abssort(float* x, unsigned n) {
    std::sort(x, x + n,
        // Lambda expression begins
        [](float a, float b) {
            return (std::abs(a) < std::abs(b));
        } // end of lambda expression
    );
}

//[capture list] (parameters) -> return-type { function body }
//capture list是捕获列表，用于指定lambda表达式所捕获的变量；parameters是参数列表；return-type是返回类型；function body是函数体。

//捕获
/*
//全局变量、全局对象不存在“捕获”的概念，因为都可以使用。
int g_max=10;

void funa()
{
    g_max=100;
}

void fun(int x)
{
    auto fa = []()->void {};//[]的捕获针对于fun函数范围内
    fa();
}

int main()
{
    auto fa = [](int x)->int    //[]的捕获针对于main主函数的范围内的局部变量(形参)
    {
        g_max+=10;//“捕获”不针对与全局变量
        return x+x;
    };//可以看作一个函数fa

    cout<<fa(10)<<endl;
}
*/

class Int
{
private:
    int value;
public:
    Int(int x=0):value(x) {cout<<"Create Int: "<<this<<endl;}
    ~Int() {cout<<"Destroy Int: "<<this<<endl;}
    Int(const Int &it):value(it.value) {cout<<"Copy Create Int:"<<this<<endl;}
    Int& operator=(const Int &it)
    {
        if(this!=&it)
        {
            value=it.value;
        }
        cout<<this<<"operator=() "<<&it<<endl;
        return *this;
    }
    void SetValue(int x)
    {
        value=x;
    }
    int GetValue() const
    {
        return value;
    }
};

void funa(int x,int y)
{
    int z=x+y;
    Int obja(10);

    auto fa=[](int a)->void 
    {
        //z=10; error
        //a=z;  error
        cout<<a<<endl;
    };
    //[]表示此lambda表达式不捕获函数funa中的任何一个变量z,x,y
    //捕获函数中的变量是不是相当于拷贝一份变量到栈中？yes下面已解决
    auto fb =[x,z]()->void
    {
        int a=z+x;//[x,z]声明捕获目标下x,z(按照值捕获)，故可以使用
        //值捕获只能进行读取操作，不能修改其值，此处的x,z是重新开辟的变量
        //z+=100;   error
        cout<<a<<endl;
    };
    fa(19);
    fb();

    //运行时只创建一个Int对象obja
}

void funb(int x,int y)
{
    int z=x+y;
    Int obja(10);//第一次构建一个obja对象，因为fa()中用到了外部对象obja，所以之后需要拷贝构造一个其副本
    Int objb(20);//虽然objb也被捕获，但是再fa()中并未用到，所以系统也不会调用拷贝构造函数构造其副本
    auto fa=[=]()->void
    {
        cout<<obja.GetValue()<<endl;
    };
    fa();//因为是[=]，值捕获此函数内的所有变量、对象，所需需要将obja复制一遍，即通过拷贝构造函数重新构造一个新的Int对象

    auto fb=[obja,x]()->void
    {
        obja.GetValue();
        //cout<<z<<endl; error   []只捕获了obja和x
        //z+=10; error  值捕获不能修改变量、对象值
    };
    
}
void func(int x,int y)
{
    int z=x+y;
    Int obja(10);
    auto fc=[&obja,&z]()->void
    {
        obja.GetValue();//以引用捕获，故不会调用拷贝构造函数构造副本，但是以引用捕获则可以在外部改动其值
        cout<<obja.GetValue()<<endl;
        cout<<z<<endl;//ok 以引用捕获
        z+=10;//ok 以引用捕获则可以修改变量、对象值
    };
    obja.SetValue(100);//引用捕获，修改了lambda表达式fc中的obja的值；若改成值引用[obja]，则会创建一个副本，原值不会被修改
    fc();
}
/*
int main()
{
    funa(10,20);//输出19，40，但是只构造一个Int对象
    funb(10,20);//输出10，因为是值捕获，所以构造两个Int对象，调用1次构造和析构函数，再通过拷贝构造函数重新构造一个新的Int对象
    //总结当使用值捕获时，当函数内并未用到外部对象时，系统并不会拷贝构造一个其副本；但是当函数运行过程中捕获到外部对象且确实用到此外部对象时，则系统会调用拷贝构造函数复制一份其副本
    //Q：能否共用一份外部对象而不复制一个副本？为什么要设计一个引用的机制？
    func(10,20);//输出100，因为为引用捕获，所以函数内所使用到的对象的值可以被改变
    
    int a=0;
    //auto f1=[=] {return a++;};//error 不能修改按值捕获的外部变量
    auto f2=[=]() mutable {return a++;};//ok
    return 0;
}
*/

typedef void (*Ptr)(int);//定义一个函数指针类型Ptr，指向一个以int为参数，返回为void的函数

int main()
{
    int a=10,b=20;
    auto f1=[](int a)->void {cout<<a<<endl;};
    auto f2=[](int x)->void {cout<<x+10<<endl;};
    //auto f3=[=]()->int {return a++;};           //error,因为f3没有使用mutable修饰符，不能修改捕获的变量
    auto f4=[=]()mutable->int {return a++;};    //ok
    auto f5(f1);    //ok
    //f1=f2;  //error,//将f2赋值给f1，产生编译错误，因为f1和f2的类型不同
    Ptr p1=f1;  //ok
    auto f6=[=](int x)->void {cout<<"hello"<<endl;};
    //Ptr p2=f4;  //error,将f4赋值给函数指针p2，产生编译错误，因为f4的类型和Ptr不匹配
}

int main1()
{
    using func_t= int (*)(int);
    func_t f=[](int a)->int {return a;};
    int x=f(123);                   //123
    cout<<typeid(f).name()<<endl;   //int(__cdecl*)(int);
    return 0;
}

//仿函数&lambda表达式
class StringAppend {
public:
    //接受一个字符串作为构造函数参数
    explicit StringAppend(const string& str) : ss(str){}
    void operator() (const string& str) const {
         cout << str << ' ' << ss << endl;
    }//将重载的函数调用运算符中将传入的字符串和构造函数中的字符串拼接起来输出
private:
    const string ss;
};
 
int main2() {
    StringAppend myFunctor2("and world!");
    myFunctor2("Hello");//输出Hello and world!
    return 0;
}

//设计仿函数，调用for_each函数将vector中的偶数打印出来
class CoountEven
{
    int &count_;
public:
    CoountEven(int &count):count_(count) {}//构造函数，初始化计数器
    void operator()(int val)    //重载函数调用符
    {
        if(!(val & 1))  //val%2==0
        {
            ++count_;
        }
    }
};

int main4()
{
    std::vector<int> v={1,2,3,4,5,6};
    int even_count=0;//偶数计数器
    for_each(v.begin(), v.end(), CoountEven(even_count));//遍历v并统计偶数个数
    cout<<"the number of even is "<<even_count<<even_count<<endl;
    return 0;
}

//使用lambda表达式实现
int main5()
{
    std::vector<int> v={1,2,3,4,5,6};
    int even_count=0;
    for_each(v.begin(), v.end(), [&even_count](int val){
        if(val%2==0)
        {
            ++even_count;
        }
    });
    std::cout<<"the number of even is "<<even_count<<std::endl;
    return 0;
}