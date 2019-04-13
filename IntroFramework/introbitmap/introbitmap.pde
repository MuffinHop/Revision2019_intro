PImage q;

final int W = 290;
final int H = 42;

byte[] pic;
String picname = "quadlogo";

void setup() {
  size(320,200);
  q = loadImage(picname+".png");
  pic = new byte[(W*H)/8];
  for (int i = 0; i < (W*H)/8; i++) pic[i] = 0;
}

int bincnt = 0;
String binstr = "";
void draw() {
  background(0);
  image(q,0,0);
  int i = 0;
  for (int y = 0; y < H; y++) {
   for (int x = 0; x < W; x++) {
     color c = q.get(x,y);
     if (red(c) > 0 || green(c) > 0 || blue(c) > 0) {
       binstr+="1";
     } else {
       binstr+="0";
     }
     bincnt++;
     if (bincnt == 8) {
       bincnt = 0; 
       pic[i++] = (byte)unbinary(binstr);
       binstr = "";
     }
  }
 }
 saveBytes(picname+".bin", pic);
 frameRate(0);
}
