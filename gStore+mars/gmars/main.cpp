/*$Id: main.cu 729 2009-11-12 09:56:09Z wenbinor $*/
/**
 *This is developed by jerry,all right reserved
 */

/******************************************************************
 * A Template application
 ******************************************************************/

#include "MarsInc.h"
#include "global.h"
//新添加的
#include<sys/time.h> 
//#include "GstoreConnector.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <tr1/unordered_map>
#include <ext/hash_map>
#include<pthread.h>
using namespace __gnu_cxx;
using namespace std;

//扩展命名空间，使得hash_map()能够处理string
namespace __gnu_cxx
{
    template<> struct hash< std::string >
    {
        size_t operator()( const std::string& x ) const
        {
            return hash< const char* >()( x.c_str() );
        }
    };

    template<> struct hash<long long>
    {
        size_t operator()(long long x) const
        {
            return x;
        }
    };
}

//函数声明
	int split(const string& str, vector<string>& ret_, const string& sep);
	string replace(const string& str, const string& src, const string& dest);
	void* thread1(void *arg);
	void* thread2(void *arg);

	void printFun(void* key, void* val, int keySize, int valSize,FILE *fp);
	void Output(Spec_t* spec, int num,FILE *fp);


	typedef pair<string,int> my_pair;
	typedef pair<int,string> rev_my_pair;
	hash_map<string,int>sub_map;
	hash_map<int,string>rev_sub_map;
	//hash_map<string,int>pre_map;
	hash_map<string,int>obj_map;
	hash_map<int,string>rev_obj_map;

	int number_sub_map=1;
	int number_obj_map=1;

	pthread_mutex_t mutex ;


	int key;
	VAL val;

	Spec_t *spec = GetDefaultSpec();

int main( int argc, char** argv) 
{
	
	//MAP_ONLY, MAP_GROUP, or MAP_REDUCE
	//制定工作流
	spec->workflow = MAP_REDUCE;
	//spec->workflow = MAP_GROUP;
	//spec->workflow=MAP_ONLY;
	//1 for outputing result to Host
	//by default, Mars keeps results in device memory
	spec->outputToHost = 1;

	//----------------------------------------------
	//preprocess
	//----------------------------------------------

        pthread_t id1;  
        pthread_t id2;
	cout<<"main()"<<endl;
    timeval tBegin, tEnd, Diff_time;  
  
    /*************获取时间起点*************/  
    	gettimeofday(&tBegin, NULL);
	long start = ((long)tBegin.tv_sec)*1000+(long)tBegin.tv_usec/1000;
	pthread_mutex_init(&mutex,NULL);  
        pthread_create(&id1,NULL,thread1,NULL);  //左表
        pthread_create(&id2,NULL,thread2,NULL);  //右表
        pthread_join(id1,NULL);  
        pthread_join(id2,NULL); 


	//----------------------------------------------
	//start mapreduce
	//----------------------------------------------
	MapReduce(spec);
    /*************获取时间终点*************/  
    gettimeofday(&tEnd, NULL);  
 	long end = ((long)tEnd.tv_sec)*1000+(long)tEnd.tv_usec/1000;

	cout<<"程序运行时间："<<(end - start)<<" ms"<<endl;
	//----------------------------------------------
	//further processing
	//----------------------------------------------
	//运行时，加第二个参数表示，输出的文件路径
	FILE *fp_o= fopen("123.txt","w");
	Output(spec, spec->outputRecordCount,fp_o);
	fclose(fp_o);

	//----------------------------------------------
	//finish
	//----------------------------------------------
	FinishMapReduce(spec);
	return 0;
}



void printFun(void* key, void* val, int keySize, int valSize,FILE *fp)
{
	PATH *path=(PATH *)key;
	int* k = (int*)val;
	//fprintf(fp,"path:(%d-%d-%d),%d\n",path->path[0],path->path[1],path->path[2], *k);
string str1(rev_sub_map.find(path->path[0])->second);
string str2(rev_obj_map.find(path->path[2])->second);
cout<<str1<<"\t"<<str2<<endl;
	//fprintf(fp,"%s    %s\n",&str1,&str2);
	//printf("(%d, %d,%d,%d,%d)\n", *k, v->x,v->y,v->z,v->w);
}

void Output(Spec_t* spec, int num,FILE *fp)
{
	if (spec->outputToHost != 1)
	{
		printf("Error: please set outputToHost to 1 first!\n");
		return;
	}
	if (num > spec->outputRecordCount) num = spec->outputRecordCount;

	PrintOutputRecords(spec, num, printFun,fp);
}


//线程1
void* thread1(void* arg)
{
    // initialize the GStore server's IP address and port.
    //GstoreConnector gc("127.0.0.1", 3305);

    // build a new database by a RDF file.
    // note that the relative path is related to gserver.
    //gc.build("mydb.db", "data/lubm10.nt");
	//gc.load("mydb.db");

    // then you can execute SPARQL query on this database.
    //std::string sparql = "select ?x ?y where \
                         { \
                         ?x    <tju:#subOrganizationOf>    ?y. \
                         }";


	//查询结果，已string的方式保存
	std::string answer = "a b\nc d\ne f\ng h";

	//保存每一行的临时数据
	string tempStr;
	cout<<"这是线程1"<<endl;//加个标记，区分下线程编号
	vector<string> str_vector;
	split(answer, str_vector,"\n");//实现对于输出结果安行切分

	vector<string>::iterator it;
	it=str_vector.begin();
	cout<<*it<<endl;
	for(it=str_vector.begin()+2;it!=str_vector.end();it++)
	{
	tempStr=*it;
	//strings实现了对于每一行的字符串进行切分
	vector<string> strings;
	split(tempStr,strings," ");//按照空格键进行切分
	vector<string>::iterator iter;
	iter=strings.begin();
	string str1=*iter;
	
	if(sub_map.find(str1)==sub_map.end())//把主语加入到sub_dic
	{pthread_mutex_lock(&mutex); //枷锁
	sub_map.insert(my_pair(str1,number_sub_map));
	//添加反向的map
	rev_sub_map.insert(rev_my_pair(number_sub_map++,str1));
	pthread_mutex_unlock(&mutex);//解锁
	}
	iter=strings.begin()+1;
	string str2=*iter;
	if(obj_map.find(str2)==obj_map.end())//把宾语加入到obj_dic
	{pthread_mutex_lock(&mutex);//加锁
	obj_map.insert(my_pair(str2,number_obj_map));
	//添加反向的map
	rev_obj_map.insert(rev_my_pair(number_obj_map++,str2));
	pthread_mutex_unlock(&mutex);//解锁
	}
	//添加到InputRecord
pthread_mutex_lock(&mutex);//加锁
		key=sub_map.find(str1)->second;
		val.label=0;
		val.value=obj_map.find(str2)->second;
//printf("label:0,key:%d,value:%d\n",key,val.value);
		AddMapInputRecord(spec, &key, &val, sizeof(int), sizeof(VAL));
pthread_mutex_unlock(&mutex);//解锁
	}
    //gc.unload("mydb.db");
	return 0;
}

//线程2
void* thread2(void* arg)
{
    // initialize the GStore server's IP address and port.
    //GstoreConnector gc("172.26.45.194", 3305);

    // build a new database by a RDF file.
    // note that the relative path is related to gserver.
    //gc.build("mydb.db", "data/lubm10.nt");
	//gc.load("mydb.db");
	//gc.load("LUBM10.db");

    // then you can execute SPARQL query on this database.
    //std::string sparql = "select ?y ?z where \
                         { \
                         ?y    <tju:#subOrganizationOf>    ?z. \
                         }";
	
	//查询结果，已string的方式保存
	std::string answer = "a b\nc d\ne f\ng z";
	//保存每一行的临时数据
	string tempStr;
	cout<<"这是线程2"<<endl;
	vector<string> str_vector;
	split(answer, str_vector,"\n");//实现对于输出结果安行切分
	vector<string>::iterator it;
	it=str_vector.begin();
	cout<<*it<<endl;
	for(it=str_vector.begin()+2;it!=str_vector.end();it++)
	{
	tempStr=*it;
	
	vector<string> strings;
	split(tempStr,strings,"\t");
	vector<string>::iterator iter;
	iter=strings.begin();
	string str1=*iter;
	if(sub_map.find(str1)==sub_map.end())
	{pthread_mutex_lock(&mutex); //枷锁
	sub_map.insert(my_pair(str1,number_sub_map));
	//添加反向的map
	rev_sub_map.insert(rev_my_pair(number_sub_map++,str1));
	pthread_mutex_unlock(&mutex);//解锁
	}

	iter=strings.begin()+1;
	string str2=*iter;
	if(obj_map.find(str2)==obj_map.end())
	{pthread_mutex_lock(&mutex);//加锁
	obj_map.insert(my_pair(str2,number_obj_map++));
	//添加反向的map
	rev_obj_map.insert(rev_my_pair(number_obj_map++,str2));
	pthread_mutex_unlock(&mutex);//解锁
	}
	//添加到InputRecord
pthread_mutex_lock(&mutex);//加锁
		key=sub_map.find(str1)->second;
		val.label=1;
		val.value=obj_map.find(str2)->second;
//printf("lable:1,key:%d,value:%d\n",key,val.value);
		AddMapInputRecord(spec, &key, &val, sizeof(int), sizeof(VAL));
pthread_mutex_unlock(&mutex);//解锁
	}
    //gc.unload("mydb.db");
	return 0;
}

//自定义的split方法
int split(const string& str, vector<string>& ret_, const string& sep)
{
    if (str.empty())
    {
        return 0;
    }

    string tmp;
    string::size_type pos_begin = str.find_first_not_of(sep);
    string::size_type comma_pos = 0;

    while (pos_begin != string::npos)
    {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != string::npos)
        {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + sep.length();
        }
        else
        {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if (!tmp.empty())
        {
            ret_.push_back(tmp);
            tmp.clear();
        }
    }
    return 0;
}



//自定义的replace方法
string replace(const string& str, const string& src, const string& dest)
{
    string ret;

    string::size_type pos_begin = 0;
    string::size_type pos = str.find(src);
    while (pos != string::npos)
    {
        //cout <<"replacexxx:" << pos_begin <<" " << pos <<"\n";
        ret.append(str.data() + pos_begin, pos - pos_begin);
        ret += dest;
        pos_begin = pos + 1;
        pos       = str.find(src, pos_begin);
    }
    if (pos_begin < str.length())
    {
        ret.append(str.begin() + pos_begin, str.end());
    }
    return ret;
}
