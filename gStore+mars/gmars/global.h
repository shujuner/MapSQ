/*$Id: global.h 732 2009-11-13 14:47:56Z wenbinor $*/
/**
 *This is the source code for Mars, a MapReduce framework on graphics
 *processors.
 *Developers: Wenbin Fang (HKUST), Bingsheng He (Microsoft Research Asia)
 *Naga K. Govindaraju (Microsoft Corp.), Qiong Luo (HKUST), Tuyong Wang (Sina.com).
 *If you have any question on the code, please contact us at 
 *           wenbin@cse.ust.hk or savenhe@microsoft.com
 *
 *The license is a free non-exclusive, non-transferable license to reproduce, 
 *use, modify and display the source code version of the Software, with or 
 *without modifications solely for non-commercial research, educational or 
 *evaluation purposes. The license does not entitle Licensee to technical support, 
 *telephone assistance, enhancements or updates to the Software. All rights, title 
 *to and ownership interest in Mars, including all intellectual property rights 
 *therein shall remain in HKUST.
 */

#ifndef __GLOBAL__
#define __GLOBAL__

typedef struct//左表
{
	int id1;
	int id2;
} LEFT;

typedef struct//右表
{
	int id1;
	int id2;
} RIGHT;//表示一条边的两个顶点

typedef struct
{
	int label;
	int value;
} VAL;

typedef struct//join 之后的路径
{
	int path[3];
} PATH;

#endif
