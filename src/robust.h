# define        DIM             512
# define        DIMH            1024
# define        FILTER          100
# define        PI              3.141592654
# define        ROUND(A)        ((A)<0.0?-((int)(-(A)+0.5)):((int)((A)+0.5)))
# define        max(A,B)        ((A)<(B)?(B):(A))
# define        min(A,B)        ((A)<(B)?(A):(B))
# define        sign(A,B)        ((A)<0?-(B):(B))
# define        abs(A)        ((A)<0?-(A):(A))

#define ROOT2 1.4142136
#define MAXLEN 256

/* function interface spec */

// warp.c
void warp_image(float *, float *, float *, float *, int , int);

// derivatives.c 
void horn_derives(float *, float *, float *,float *, float *, int, int);
void horn_dt(float *, float *, float *, int, int);
void laplacian2(float *, float *,int,int);

// disconts.c
void data_disconts(float *, float *, float *,float *, float *, int, int, float *, float *, float *);
void spatial_disconts(float *, float *, float *, int, int, float *);

// filter.c
void add_image_value(float *, float, int, int);

// image-io.c
void save_image_float(char *, float *, int, int);
void save_pgm(char *, float *, int, int);
void save_pgm_no_scale(char *, float *, int, int);

// pyramid.c
void project_flow(float *, float *, int, int);
void reduce_flow(float *, float *, int, int);
void reduce_image(float *, float *,int, int);

// pyramid-sor.c
void pyramid_sor(float *, float *, int, int, int, float, float *, float *, float *, float *, float *, float *, float, float, float, float *, float *, float *, int, int, float *, float *, float *, float *,float *, float *, int);

// sor.c
void fix_flow(float *, int, int);
void inten_const(float *, float *, float *,float *, float *, float *, int, int);
void sor(float *, float *, int, float, float *, float *, float *, float *, float *, float *, float, float, float,float *, float *, float *, int, int, float *, float *, float *, float *, float *, float *);
void sor_1_iter(float *, float *, float *, float *, float *, int, int, float, float *,float, float, float,float *, float *, float *);
void sor_scale(float *, float *, int, int, float *, float *, float *,float, float, float);

// utils.c
void add_image(float *, float *, int, int);
void copy_image(float *, float *, int, int);
void error(char *, char *);
void read_image_strip_bytes(float *, char *, int, int, int);
