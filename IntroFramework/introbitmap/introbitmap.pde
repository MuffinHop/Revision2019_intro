PImage q;

final int W = 516;
final int H = 212;

byte[] pic;
String picname = "logoja";

void setup() {
  size(640,360);
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
  for (int y = H-1; y >= 0; y--) {
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
