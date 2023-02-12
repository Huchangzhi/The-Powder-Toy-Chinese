#pragma once
#include "Config.h"
#include "common/String.h"

inline ByteString IntroText()
{
	ByteStringBuilder sb;
	sb << "\bl\bU" << APPNAME << "\bU - Version " << SAVE_VERSION << "." << MINOR_VERSION << " - https://powdertoy.co.uk, irc.libera.chat #powder, https://tpt.io/discord\n"
	      "\n"
	      "\n"
		"\bgCtrl+C/V/X \u590d\u5236\u002c\u7c98\u8d34\u002c\u526a\u5207\n"
		"\bg\u9f20\u6807\u79fb\u81f3\u53f3\u4fa7\u5143\u7d20\u680f\u4e2d\u67d0\u4e00\u4e2a\u7c7b\u522b\u53ef\u4ee5\u663e\u793a\u8be5\u7c7b\u4e0b\u7684\u6240\u6709\u5143\u7d20\n"
		"\bg\u5229\u7528\u9f20\u6807\u5de6\u002f\u53f3\u952e\u9009\u53d6\u9700\u8981\u7684\u5143\u7d20\n"
		"\u4f7f\u7528\u9f20\u6807\u5de6\u002f\u53f3\u952e\u7ed8\u5236\n"
		"\u4f7f\u7528\u0053\u0068\u0069\u0066\u0074\u002b\u62d6\u52a8\u9f20\u6807\u7ed8\u5236\u76f4\u7ebf\n"
		"\u4f7f\u7528\u0043\u0074\u0072\u006c\u002b\u62d6\u52a8\u7ed8\u5236\u77e9\u5f62\n"
		"\u0043\u0074\u0072\u006c\u002b\u0053\u0068\u0069\u0066\u0074\u002b\u5355\u51fb\u586b\u5145\u5c01\u95ed\u533a\u57df\n"
		"\u4f7f\u7528\u9f20\u6807\u6eda\u8f6e\u6216\u0027\u005b\u0027\u0061\u006e\u0064\u0027\u005d\u0027\u8c03\u6574\u7b14\u5237\u5927\u5c0f\n"
		"\u4f7f\u7528\u9f20\u6807\u4e2d\u952e\u6216\u6309\u4f4f\u0041\u006c\u0074\u4f7f\u7528\u5438\u7ba1\u5de5\u5177\n"
		"\u4f7f\u7528\u0043\u0074\u0072\u006c\u002b\u005a\u0020\u64a4\u9500\n"
		"\n\bo\u6309\u4f4f\u0027\u005a\u0027\u952e\u6253\u5f00\u653e\u5927\u955c\u002c\u4f7f\u7528\u9f20\u6807\u6eda\u8f6e\u8c03\u6574\u500d\u6570\u002c\u9f20\u6807\u5de6\u952e\u56fa\u5b9a\u653e\u5927\u533a\u57df\u002c\u56fa\u5b9a\u540e\u53ef\u76f4\u63a5\u5728\u653e\u5927\u533a\u57df\u7ed8\u5236\n"
		"\u6309\u4e0b\u7a7a\u683c\u952e\u4ee5\u6682\u505c\u002c\u6309\u4e0b\u0027\u0046\u0027\u53ef\u4ee5\u5e27\u8fdb\n"
		"\u6309\u4e0b\u0027\u0053\u0027\u952e\u9009\u62e9\u533a\u57df\u4fdd\u5b58\u5230\u526a\u8d34\u677f\u002c\u6309\u4e0b\u0027\u004c\u0027\u952e\u52a0\u8f7d\u6700\u8fd1\u4fdd\u5b58\u7684\u5bf9\u8c61\u002c\u6309\u4e0b\u0027\u004b\u0027\u952e\u6d4f\u89c8\u526a\u8d34\u677f\n"
		"\u6309\u4e0b\u0027\u0050\u0027\u952e\u622a\u56fe\u002c\u622a\u56fe\u5c06\u88ab\u4fdd\u5b58\u5230\u6e38\u620f\u76ee\u5f55\u4e0b\n"
		"\u6309\u4e0b\u0027\u0048\u0027\u6253\u5f00\u002f\u5173\u95ed\u0048\u0055\u0044\u002e\u6309\u4e0b\u0027\u0044\u0027\u5728\u0048\u0055\u0044\u6253\u5f00\u002f\u5173\u95ed\u8c03\u8bd5\u6a21\u5f0f\u002e\n"
		"\n"
		"\u8d21\u732e\u8005: \bgStanislaw K Skowronek (\u6700\u521d\u8bbe\u8ba1\u8005),\n"
		"\bgSimon Robertshaw, Skresanov Savely, cracker64, Catelite, Bryan Hoyle, Nathan Cousins, jacksonmj,\n"
		"\bgFelix Wallin, Lieuwe Mosch, Anthony Boot, Me4502, MaksProg, jacob1, mniip, LBPHacker\n"
		"\n"
		"\bo\u6c49\u5316: \bgDragonRSTER\n"
		"\bo\u7a0b\u5e8f: \bgDragonRSTER Github:\br https://github.com/Dragonrster/The-Powder-Toy-Chinese\n"
		"\n";
	if constexpr (BETA)
	{
		sb << "\brThis is a BETA, you cannot save things publicly, nor open local saves and stamps made with it in older versions.\n"
		      "\brIf you are planning on publishing any saves, use the release version.\n";
	}
	else
	{
		sb << "\bg\u8981\u4f7f\u7528\u4fdd\u5b58\u7b49\u5728\u7ebf\u529f\u80fd\u002c\u60a8\u9700\u8981\u5728\u4ee5\u4e0b\u4f4d\u7f6e\u6ce8\u518c: \brhttps://powdertoy.co.uk/Register.html\n";
	}
	sb << "\n"
	   << "\bt" << SAVE_VERSION << "." << MINOR_VERSION << "." << BUILD_NUM << " " << IDENT;
	if constexpr (SNAPSHOT)
	{
		sb << " SNAPSHOT " << SNAPSHOT_ID;
	}
	else if constexpr (MOD)
	{
		sb << " MODVER " << SNAPSHOT_ID;
	}
	if constexpr (LUACONSOLE)
	{
		sb << " LUACONSOLE";
	}
#ifdef REALISTIC
	sb << " REALISTIC";
#endif
	if constexpr (NOHTTP)
	{
		sb << " NOHTTP";
	}
	if constexpr (DEBUG)
	{
		sb << " DEBUG";
	}
	if constexpr (ENFORCE_HTTPS)
	{
		sb << " HTTPS";
	}
	return sb.Build();
}
