#include "LDexFile.H"
/*
************************************************************
*			一次运行DUMP出来三个文件
*版本1.直接DUMP数据
*版本2.延时DUMP数据
*版本3.根据DexFile之修复Class Data Sources Offset
//*版本2.Dump出dexFileParse函数返回的DexFile指针
*版本3.原本调用dexFileParse函数调用的内存和宽度，在利用dexFileParse分析，在DUMP
************************************************************
*/
void* DexFile_Parse(void* in){
	//获取传递参数，并打印数据
	DumpInfo* Info = (DumpInfo*)in;
	DEXLOG("延时10S!");
	//sleep(0);
	DEXLOG("正在回写DexHeader数据，防止解析后加固修改Header,为了防止错误基本和数据用MAPoff里面数据!");
	memcpy(Info->addr,Info->BackOldDex,0x70);
	DEXLOG("下载Demo2!");
	LDex_Util::SaveFile(Info->addr,Info->len,AppName,LDex_Util::GetTimeName("Demo2"));
	DEXLOG("下载Demo3，先解码DexFile然后合并!");
	memcpy((void*)Info->Dex->pHeader,Info->BackOldDex,0x70);
	LDexFile* parse = (LDexFile*)Info->DexParse;
	parse->DumpToFile(AppName,LDex_Util::GetTimeName("Demo3"));
	return NULL;
}
/*
************************************************************
*				Dump_DexFile
* @param	inAddr -> 指向DexFile
* @cmt	创建一个线程，然后用线程去执行解码 DexFile
************************************************************
*/
void DexFile_Dump(void* inAddr,size_t inLen,void* inDex){
	DEXLOG("inDex:0x%08X,length:0x%08X,DexFile:0x%08X",inAddr,inLen,inDex);
	if(!LDex_Util::isDex(inAddr)){
		DEXLOG("[ERR]输入格式出现错误，无法识别DEX或DEY,程序自动退出!");
		return;
	}
	DEXLOG("输入格式正确，开始打印相关数据!");
	Dex_Header::Log(inAddr);
	Dex_Maps::Log_Dex(inAddr);
	//开始Dump最原始导入数据，防止数据变化先Dump后运行程序
	DEXLOG("开始自动脱壳!");
	DEXLOG("Dump_DexFile@Dump Demo 1!");
	LDex_Util::SaveFile(inAddr,inLen,AppName,LDex_Util::GetTimeName("Demo1"));
	DEXLOG("Dump_DexFile@创建子线程!");
	DumpInfo* info = (DumpInfo*)malloc(sizeof(DumpInfo)+1);
	memset(info,0,sizeof(DumpInfo)+1);
	info->addr = LDex_Util::GetBase(inAddr);
	info->len = inLen;
	info->Dex = (DexFile*)inDex;
	info->BackOldDex = LDex_Util::Alloc(inLen);
	memcpy(info->BackOldDex,LDex_Util::GetBase(inAddr),0x70);
	LDexFile* parse = new LDexFile(info->addr,info->Dex);
	info->DexParse = (void*)parse;
	pthread_t thread;
	pthread_create(&thread,NULL,DexFile_Parse,info);
}
