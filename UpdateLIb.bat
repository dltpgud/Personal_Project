xcopy	/y	.\Engine\Bin\Engine.dll		.\Client\Bin\		
xcopy	/y	.\Engine\Bin\Engine.lib		.\EngineSDK\Lib\	


xcopy	/y	.\Engine\Bin\fmod.dll		.\Client\Bin\		
xcopy	/y	.\Engine\Bin\fmod_vc.lib	.\EngineSDK\Lib\	
xcopy	/y	.\Engine\Bin\fmodL.dll		.\Client\Bin\		
xcopy	/y	.\Engine\Bin\fmodL_vc.lib	.\EngineSDK\Lib\


xcopy	/y	.\Engine\ThirdPartyLib\DirectXTK.lib	.\EngineSDK\Lib\
xcopy	/y	.\Engine\ThirdPartyLib\DirectXTKd.lib	.\EngineSDK\Lib\

xcopy	/y	.\Engine\ThirdPartyLib\Effects11.lib	.\EngineSDK\Lib\
xcopy	/y	.\Engine\ThirdPartyLib\Effects11d.lib	.\EngineSDK\Lib\


xcopy	/y		.\Engine\Bin\ShaderFiles\*.*		.\Client\Bin\ShaderFiles\

xcopy	/y		.\Engine\Bin\Engine.dll		.\Tool\Bin\	
xcopy	/y/s	.\Engine\Public\*.*			  .\EngineSDK\Inc\

xcopy   /y/s    .\Tool\Bin\Data\*.*          .\Client\Bin\Data\
xcopy   /y/s    .\Tool\Bin\Resources\*.*     .\Client\Bin\Resources\     