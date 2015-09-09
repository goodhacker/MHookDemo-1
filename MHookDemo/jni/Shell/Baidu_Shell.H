#ifndef __Baidu_Shell_H_
#define __Baidu_Shell_H_
/*
*									百度加固说明
*1.此脱壳为脱百度 2代和3代结合版本
*2.加密流程:
*	采用动态加载assets下的baiduprotect.jar
*	然后采用重写onCreate,用onCreate001代替，onCreate内容为修复onCreate001代码、执行onCreate001代码、清楚onCreate001代码。
*	修复代码不能连续运行两次，
*3.采用Hook DexParse来获取Dex相关数据，然后遍历ClassDef将所有onCreate001类直接解码
*4.Dump出修复好的Dex
*5.然而Dump的Dex还要修复(可能可以根据ClassDef自动修改)
*/
void Dump_Baidu(){

}

#endif
