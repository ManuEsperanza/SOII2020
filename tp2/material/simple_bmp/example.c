#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <libnet.h>

#include "simple_bmp.h"

#define  SIZE_K 43

uint64_t rdtsc ();
void kernel_setup (uint16_t **kern, int16_t ksize);
int main ()
{
    uint16_t **kernel = calloc (SIZE_K, sizeof (int *));
    uint32_t color,rojo,azul,verde; //las utilizo como variable intermedia por si la variable de algún color desborda por ser mayor a 255.
    uint32_t ponderacion = 0;
    signed int bordesX = -SIZE_K/2;
    signed int bordesY = -SIZE_K/2;
    float k = 1.5;
    float l = 12;
    for (int k = 0; k < SIZE_K; k++)
    kernel[k] = calloc (SIZE_K, sizeof (uint16_t));
    kernel_setup (kernel, SIZE_K);

    /*sbmp_image imgtest = {0};
    int32_t check = sbmp_initialize_bmp (&imgtest, 121, 123);

    if (SBMP_OK != check)
    {
        exit (-1);
    }*/
    char * dirImagen = "base.bmp";
    sbmp_image base = {0};
    int32_t check = sbmp_load_bmp(dirImagen,&base);

    if (SBMP_OK != check)
    {
        exit (-1);
    }
    for (int i = 0; i < base.info.image_height; ++i)
    {
        for (int j = 0; j < base.info.image_width; ++j)
        {
            if ((((i-(base.info.image_height/2)) * (i-(base.info.image_height/2))) + ((j-(base.info.image_width/2)) * (j-(base.info.image_width/2)))) <= 2250000)
            {   //filtro de brillo y contraste
                color = ((base.data[i][j].red * k) + l);
                if (color > 255)
                {
                    base.data[i][j].red = 255;
                }
                else
                {
                    base.data[i][j].red = color; 
                }
                color = ((base.data[i][j].blue * k) + l);
                if (color > 255)
                {
                    base.data[i][j].blue = 255;
                }
                else
                {
                    base.data[i][j].blue = color; 
                }
                color = ((base.data[i][j].green * k) + l);
                if (color > 255)
                {
                    base.data[i][j].green = 255;
                }
                else
                {
                    base.data[i][j].green = color; 
                }
            }
            else    //filtro blur
            {
                if ( ( i > SIZE_K/2 &&  (i < (base.info.image_height - SIZE_K/2))) && ( j > SIZE_K/2) &&  (j < (base.info.image_width - SIZE_K/2)))   //todo lo que no sea borde
                { 
                    rojo  = 0;
                    azul  = 0;
                    verde = 0;
                    ponderacion = 0;
                    for (int a = 0; a < SIZE_K; a++)
                    {
                        for (int b=0; b < SIZE_K; b++)
                        {             
                            rojo  += (kernel[a][b]* base.data[(i + bordesY)][(j + bordesX)].red);
                            azul  += (kernel[a][b]* base.data[(i + bordesY)][(j + bordesX)].blue);
                            verde += (kernel[a][b]* base.data[(i + bordesY)][(j + bordesX)].green);
                            ponderacion += kernel[a][b];
                            
                            if (bordesX == SIZE_K/2)
                                bordesX = -SIZE_K/2;
                            else
                                bordesX++;      
                        }
                        if (bordesY == SIZE_K/2)
                            bordesY = -SIZE_K/2;
                        else
                            bordesY++;          
                    }
                    base.data[i][j].red = rojo/ponderacion;
                    base.data[i][j].blue = azul/ponderacion;
                    base.data[i][j].green = verde/ponderacion;
                }
            }
        }
    }
    sbmp_save_bmp ("testeo.bmp", &base);
}

uint64_t rdtsc ()
{
  unsigned int lo, hi;
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return ((uint64_t) hi << 32) | lo;
}

void kernel_setup (uint16_t **kern, int16_t ksize)
{
  uint16_t st_val = 1;
  for (int j = 0; j < ksize; j++)
    kern[0][j] = st_val;

  for (int i = 1; i < ksize / 2 + 1; i++)
    {
      for (int j = 0; j < ksize; j++)
        {
          if (j >= i && j < (ksize - i))
            kern[i][j] = (uint16_t) (kern[i - 1][j] + (uint16_t) 1);
          else
            kern[i][j] = kern[i - 1][j];
        }

    }
  for (int i = 1; i < ksize / 2; i++)
    {
      for (int j = 0; j < ksize; j++)
        {
          kern[i + ksize / 2][j] = kern[ksize / 2 - i][j];
        }

    }
}
