typedef unsigned char *image_ptr;
typedef double *double_ptr;
typedef struct
    {
    unsigned char r,g,b;
    } pixel;

typedef pixel *pixel_ptr;

typedef struct
    {
    int width;
    int height;
    float *x_data;
    float *y_data;
    } mesh;

typedef struct
    {
    double re;
    double im;
    } COMPLEX;

typedef COMPLEX *complex_ptr;

typedef struct
    {
    int x;
    int y;
    } POINT;

typedef struct 
    {
    POINT P;
    POINT Q;
    int dx, dy;
    float length;
    long length_squared;
    } LINE;

typedef struct 
    {
    POINT P;
    POINT Q;
    } LINE_SEGMENT;

typedef struct 
    {
    int number;
    LINE_SEGMENT line[100];
    char *filename;
    } LINE_LIST;

typedef unsigned short int USHORT;
typedef unsigned long int  ULONG;

typedef struct {
    USHORT bmpType;                       // BMP 파일의 종류를 나타내는 식별자
    ULONG  bmpSize;                       // BMP 파일의 크기(바이트 단위)
    USHORT bmpReserved1;                  // 예약
    USHORT bmpReserved2;                  // 예약
    ULONG  bmpOffset;                     // 이미지 정보가 시작되는 위치(Offset)
    ULONG  bmpHeaderSize;                 // Header의 크기(바이트 단위)
    ULONG  bmpWidth;                      // 가로 크기(픽셀 단위)
    ULONG  bmpHeight;                     // 세로 크기(픽셀 단위)
    USHORT bmpPlanes;                     // Color Plane의 수(1로 셋팅)
    USHORT bmpBitCount;                   // 한 픽셀에 할당된 비트수
    ULONG  bmpCompression;                // 압축 기법 사용여부
    ULONG  bmpBitmapSize;                 // 비트맵 이미지의 크기
    ULONG  bmpXPelsPerMeter;              // 수평 해상도
    ULONG  bmpYPelsPerMeter;              // 수직 해상도
    ULONG  bmpColors;                     // 사용된 색상수
    ULONG  bmpClrImportant;
} BITMAPHEADER;

#define PI   3.14159265358979323846
#define CLIP(val, low, high) {if(val<low) val=low; if(val>high) val=high;}
#define CLAMP(val, low, high) ((val<low) ? low : ((val>high) ? high : val))
#define MAX(A,B)        ((A) > (B) ? (A) : (B))
#define MIN(A,B)        ((A) < (B) ? (A) : (B))
#define IP_MALLOC(X) malloc(X)
#define IP_FREE(X) free(X)
#define PBM 4
#define PGM 5
#define PPM 6
