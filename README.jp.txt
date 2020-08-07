 =================================================================
 tinymt ver. 1.1.2
 2020.5.13

 Tiny Mersenne Twister(tinymt).
 Pseudo random number generators whose periods are 2^127-1.

 Mutsuo Saito (Hiroshima University) and
 Makoto Matsumoto (The University of Tokyo)

 Copyright (C) 2011 Mutsuo Saito, Makoto Matsumoto,
 Hiroshima University and The University of Tokyo.
 All rights reserved.

 The 3-clause BSD License is applied to this software, see
 LICENSE.txt
 =================================================================

 ドキュメントは、たとえ英語が文法的に正しくない場合でも、英語版が正式な
 ものです。

 このアーカイブファイルは二種類のソースファイルを含んでいます。
 ひとつは tinymt で、もうひとつは tinymtdc です。

 tinymt は C言語で書かれています。
 stdint.h と inttypes.hが必要です
 C99 をサポートしているコンパイラなら、標準インクルードパスにこの二つのヘッダ
 ファイルが含まれています。
 Microsoft Visual Studio 用の stdint.h と inttypes.h は Google code
 の msinttypes から入手可能です。

 tinymtdc はC++言語で書かれています。
 C++ tr1 をサポートしているコンパイラが必要です。
 また、stdint.h と inttypes.h も使用しています。
 さらに、Victor Shoup 氏のライブラリ NTL が必要です。

 tinymtもtinymtdcもdoxygenでドキュメントが生成できるように書かれています。
 doxygenがなくても実行には影響がありませんが、doxygenがあれば
 ソースファイル中の各関数について詳細なドキュメントを見ることが出来ます。

 ソースコードを変更したりしなかったりしてこのプログラムを再配布したい人
 は、LICENSE.txt（英語）を読んで下さい。

 ファイルに変更を加えて再配布する場合は、どうか、配布物の中にあなたの連
 絡先を書いておいて、問題があったときは、私たちではなく、あなたにまず連
 絡するように利用者に伝えて下さい。
