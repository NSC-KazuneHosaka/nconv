#ifndef NCONV_H
#define NCONV_H

#include <stdio.h>

/* 文字列<sourceFile>が持つ文字列の名前のファイルを入力、 */
/* 文字列<outputFile>と等しい名前を持つファイルを出力として、 */
/* sourceFileを変換した結果をoutputFileに出力する。 */
/* 変換に成功した場合、outputFileに書き込んだバイト数を返す。 */
/* 変換に失敗した場合、エラーコードまたは0を返す。 */
unsigned int npsconvert(const wchar_t* const sourceFile, const wchar_t* const outputFile);

#endif