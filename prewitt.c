#define WIDTH 256
#define HEIGHT 256
#define amp 1  // エッジ強度の倍率

#include <stdio.h>
#include <stdlib.h>
#include <math.h>   // sqrtを使うために必要

typedef struct color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} COLOR;

COLOR Edge[WIDTH][HEIGHT];

int main(void){
  int tmp;    // 一時記憶用変数
  int i, j;   // カウンタ用変数
 
  COLOR data[WIDTH][HEIGHT];     // 入力用配列
  COLOR outdata[WIDTH][HEIGHT];  // 出力用配列

  FILE *fp;             // 入力ファイルポインタ
  FILE *out;            // 出力ファイルポインタ
  char header[54];      // ヘッダー

  if ((fp = fopen("image.bmp", "rb")) == NULL) {
    fprintf(stderr, "file open error");   // "image.bmp"が存在しなかったらエラー表示
    exit(1);   // プログラム終了させる
  }

  if ((out = fopen("out_prewitt.bmp", "wb")) == NULL) {
    fprintf(stderr, "file open error");
    exit(1);   // プログラム終了させる
  }

  fread(header, 1, 54, fp);  // ヘッダーの読み込み(ヘッダー(54bit)を飛ばす) 

  for (j = HEIGHT-1; j >= 0; j--) {
    for (i = 0; i < WIDTH; i++) {
       data[i][j].b = getc(fp);
       data[i][j].g = getc(fp);
       data[i][j].r = getc(fp);
    }
  }

  fclose(fp);

  for (i = 0; i < WIDTH; i++) {
    for (j = 0; j < HEIGHT; j++) {
       outdata[i][j].r = data[i][j].r;
       outdata[i][j].g = data[i][j].g;
       outdata[i][j].b = data[i][j].b;
    }
  }
	
/*--------------------------------------
  カラー画像に対してGrayScale化を行う
--------------------------------------*/

  for(i = 0;i < WIDTH;i++) {
    for(j = 0;j < HEIGHT;j++) {
      tmp = data[i][j].r * 0.299 + data[i][j].g * 0.587 + data[i][j].b * 0.114;
      outdata[i][j].r = tmp;
      outdata[i][j].g = tmp;
      outdata[i][j].b = tmp;
    }
  }

/*--------------------------------------------
  グレイスケール化した画像をさらに
  エッジ検出する(Prewitt)
--------------------------------------------*/

  /* x軸方向にPrewittオペレータ */
  static int cx[9] =
     {-1,0,1,
      -1,0,1,
      -1,0,1};

  /* y軸方向にPrewittオペレータ */
  static int cy[9] =
      {-1,-1,-1,
		    0,0,0,  
		    1,1,1};
  int d[9];  // 3*3行列
  int I,J,edge_int;   // I,J:カウンタ用変数  edge_int:エッジ強度変数
  float xx,yy,zz;   // xx,yy:x,y方向エッジ強度データ zz:エッジ強度の値(sqrt:平方根)

  for(I=1;I<HEIGHT-1;I++){
    for(J=1;J<WIDTH-1;J++){
      d[0]=outdata[I-1][J-1].b;
      d[1]=outdata[I-1][J].b;
      d[2]=outdata[I-1][J+1].b;
      d[3]=outdata[I][J-1].b;
      d[4]=outdata[I][J].b;
      d[5]=outdata[I][J+1].b;
      d[6]=outdata[I+1][J-1].b;
      d[7]=outdata[I+1][J].b;
      d[8]=outdata[I+1][J+1].b;

      xx=(float)(cx[0]*d[0]+cx[1]*d[1]+cx[2]*d[2]+cx[3]*d[3]+cx[4]*d[4]
                   +cx[5]*d[5]+cx[6]*d[6]+cx[7]*d[7]+cx[8]*d[8]);
      yy=(float)(cy[0]*d[0]+cy[1]*d[1]+cy[2]*d[2]+cy[3]*d[3]+cy[4]*d[4]
                   +cy[5]*d[5]+cy[6]*d[6]+cy[7]*d[7]+cy[8]*d[8]);
      zz=(float)(amp*sqrt(xx*xx+yy*yy));

      edge_int=(int)zz;
      if(edge_int>255){   // 255よりも大きかったとき
        edge_int=255;   // edge_intに255を格納 
      }
      
      //エッジ強度を符号なし文字型に変換し出力画像配列に代入
      Edge[I][J].r=(unsigned char)edge_int;
      Edge[I][J].g=(unsigned char)edge_int;
      Edge[I][J].b=(unsigned char)edge_int;
    }
  }

/*-------------------------------------------
  加工処理した画像を構成する
--------------------------------------------*/

  fwrite(header, 1, 54, out);  // ヘッダーの書き込み(ヘッダー(54bit)を飛ばす) 

  // 画像の出力
  for (j = HEIGHT-1; j >= 0; j--) {
    for (i = 0; i < WIDTH; i++) {
      fputc(Edge[i][j].b, out);
      fputc(Edge[i][j].g, out);
      fputc(Edge[i][j].r, out);
    }
  }
  fclose(out);

  return 0;
}
