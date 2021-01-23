#include <stdio.h>
#include <stdlib.h>
#define LINEAS_CACHE 4	// Numero de lineas de la cache
#define BYTES_LINEA 8	// Bytes de la linea
#define TAM_RAM 1024	// bytes de la RAM
#define TAM_MAX 100	// tamaño maximo del texto
#define DEF 0		// Valor por defecto

typedef struct {
        short int ETQ;
        short int Datos[8];
} T_LINEA_CACHE;

void inicializar_cache(T_LINEA_CACHE *cache){		// inicializacion de los de datos de la cache
	for(int i = 0; i < LINEAS_CACHE; i++){
                cache[i].ETQ = 0xFF;
                for(int j = 0; j < BYTES_LINEA; j++){
                        cache[i].Datos[j] = 0;
                }
        }
}

void imprimir_cache(T_LINEA_CACHE *cache){		// imprime los datos de la cache
        for(int i = 0; i < LINEAS_CACHE; i++){
                printf("ETQ:%X  DATOS",cache[i].ETQ);
                for(int j = 7; j >= 0; j--){
                        printf(" %X",cache[i].Datos[j]);
                }
                printf("\n");
        }
}


void cargar_bloque_cache(T_LINEA_CACHE *cache, unsigned char *RAM, int *bloque, int *linea, int *etiqueta){ // carga el bloque de la RAM en la linea de la cache
                        cache[*linea].ETQ = *etiqueta;
                        for(int i = 0; i < 8; i++){
                        	switch(i){
                                	case 7: cache[*linea].Datos[i] = RAM[*bloque << 3];
                                                break;
                                        case 6: cache[*linea].Datos[i] = RAM[((*bloque << 3) | 0x1)];
                                                break;
                                        case 5: cache[*linea].Datos[i] = RAM[((*bloque << 3) | 0x2)];
                                                break;
                                        case 4: cache[*linea].Datos[i] = RAM[((*bloque << 3) | 0x3)];
                                                break;
                                        case 3: cache[*linea].Datos[i] = RAM[((*bloque << 3) | 0x4)];
                                                break;
                                        case 2: cache[*linea].Datos[i] = RAM[((*bloque << 3) | 0x5)];
                                                break;
                                        case 1: cache[*linea].Datos[i] = RAM[((*bloque << 3) | 0x6)];
                                                break;
                                        case 0: cache[*linea].Datos[i] = RAM[((*bloque << 3) | 0x7)];
                                                break;
                                }
                	}
}

void imprimir_final(int *numaccesos, int *numfallos, int *t, char *texto){ // imprime los valores finales 
	printf("Numero de accesos: %d\nNumero de fallos: %d\nTiempo medio de acceso:%0.2f\nTexto leido desde la cache: ", *numaccesos, *numfallos, (float)*t/(float)*numaccesos);
        for(int i = 0; i < *numaccesos; i++)
        	printf("%c", texto[i]);
        printf("\n");
}

int main(){
	FILE *fichero;
	unsigned char RAM[TAM_RAM];
	char texto[TAM_MAX];
	int addr = DEF, etiqueta = DEF, linea = DEF, palabra = DEF, bloque = DEF;
	T_LINEA_CACHE cache[LINEAS_CACHE];
	int tiempoglobal = DEF, numfallos = DEF, numaccesos = DEF, t = DEF;
	inicializar_cache(cache);
	fichero = fopen("RAM.bin","r");
	if(fichero == NULL){
                printf("No se ha podido leer el fichero RAM.bin.\n");
                exit(-1);
        }
        else{
                fread(RAM, sizeof(RAM), 1, fichero);
                fclose(fichero);
        }
	fichero = fopen("accesos_memoria.txt","r");
	if(fichero == NULL){
		printf("No se ha podido leer el fichero accesos_memoria.txt.\n");
		exit(-1);
	}
	else{
		while(fscanf(fichero, "%X", &addr) != EOF){
			etiqueta = addr >> 5; // desplazamiento para obtener los 11 bits que forman la etiqueta
			bloque = addr >> 3; // desplazamiento para obtener los 13 bits que forman el bloque
			linea = (addr % 32) >>3; // desplazamiento para obtener los 2 bits de la linea
			palabra = addr % 8; // desplazamiento para obtener los 3 bits de la palabra ç
			if(cache[linea].ETQ == etiqueta){
				printf("T: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X \n", t, addr, etiqueta, linea, palabra, cache[linea].Datos[palabra]);
				texto[numaccesos]  = cache[linea].Datos[palabra];
				numaccesos++;
			}
			else{
				numfallos++;
				tiempoglobal += 10;
				printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X\nCargando el bloque %02X en la linea %02X\n", t, numfallos, addr, etiqueta, linea, palabra, bloque, bloque, linea);
				cargar_bloque_cache(cache, RAM, &bloque, &linea, &etiqueta);
			}
			imprimir_cache(cache);
			sleep(2);
			t++;
		}
		fclose(fichero);
		imprimir_final(&numaccesos, &numfallos, &t, texto);
	}

	return 0;
}
