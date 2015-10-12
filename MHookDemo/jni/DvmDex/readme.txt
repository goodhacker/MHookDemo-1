/*	
					这里解析内存中的DvmDex指针
1.特点说明:
	A.将Dex解析后，在调用dvmResolveClass函数分解时，在传入参数ClassObject* 下
		中Method*指针中的insns为程序Dalvik代码,修改此指针可以修改代码执行内容。
		达到加固恢复还原后可以不修改外壳数据内容就可以修复程序。
	B.
3.
*/