#pragma once

using namespace std;

string chat_id = "-893691554";
u8string  IDemoji = u8"\U0001F194";
u8string  IPemoji = u8"\U00002699";
u8string  OSemoji = u8"\U0001F5A5";
u8string  flagEmoji = u8"\U0001F3F3";
u8string  pcnameEmoji = u8"\U0001f4bb";
u8string  greendotEmoji = u8"\U0001F7E2";
u8string  reddotEmoji = u8"\U0001F534";
u8string  ratEmoji = u8"\U0001F400";
u8string  dollarEmoji = u8"\U0001F4B2";
u8string  userEmoji = u8"\U0001F4C0";
u8string  privilegesEmoji = u8"\U0001F511";

// instead of returning a string we return a u8 string in its place. We will later reconver into a string.
// std::string resulttitle()
u8string resulttitle()
{

	u8string  r = u8"\U0001d681";
	u8string  a = u8"\U00000040";
	u8string  t = u8"\U0001d683";
	
	u8string  s = u8"\U00000024";
	u8string  h = u8"\U0001d677";
	u8string  i = u8"\U0001d678";
	u8string  t2 = u8"\U0001d683";

	u8string  l = u8"\U0001d67b";
	u8string  o = u8"\U0001d67e";
	u8string  g = u8"\U0001d676";
	u8string  s2 = u8"\U0001d682";

	return r + a + t + u8" " + s + h + i + t2 + u8" " + l + o + g + s2;
};
