

//// xcopy /�ɼ� .������������ �ִ� ���			.����������

xcopy /y/s .\Engine\Public\*.*				.\Reference\Headers\
xcopy /y .\Engine\Bin\*.lib					.\Reference\Librarys\
xcopy /y .\ThirdParty\*.lib				.\Reference\Librarys\

xcopy /y .\Engine\Bin\*.dll					.\Client\Bin\
xcopy /y .\Engine\Bin\ShaderFiles\*.hlsl	.\Client\Bin\ShaderFiles\

xcopy /y .\Engine\Bin\*.dll					.\Server\Bin\

	
xcopy		/y		.\ServerCore\Public\*.h						.\Reference\Headers\
xcopy		/y		.\ServerCore\Public\*.h						.\Engine\ThirdPartyLib\Headers\

xcopy		/y		.\ServerCore\Bin\*.lib						.\Engine\ThirdPartyLib\
xcopy		/y		.\ServerCore\Bin\*.lib						.\Reference\Librarys\		
