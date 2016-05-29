#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "math.h"

#define SUMA 0
#define RESTA 1

/*cuestiones*/
/*

-hacer subir el random cada ciertos ciclos
-penalizar por pesos
-si no cumple regla aritmetica, alejar

*/

/*operadores*/
const char *toperador1;
const char *toperador2;
const char *resultado;

/*constantes*/
int n_luc;/*numero total de luciernagas*/
int n_gen = 1000;/*numero total de generaciones*/

char array_letras[10];/*letras que no se repiten*/
int cant_letras;/*cantidad de letras que no se repiten*/



typedef struct 
{

	int luc_numeros[10];
	float luc_intensidad;
	
}sfirefly;


sfirefly *luciernagas;


/*funciones*/
void generar_numeros(int array[10]);

float calcular_func_objetivo(int operacion,int array_num[10]);

int calcular_operador(const char *operador,int array_num[10]);

int *calcular_distancia(int array1[10],int array2[10]);

void acercar_luciernaga(int array_mayor[10],int array_menor[10],int distancia[10]);

void ordenar_luciernagas();




GtkWidget *make_entry_with_label(GtkTable *table, const gchar *label_text, gint left_attach, gint right_attach, gint top_attach, 
                                gint bottom_attach, gint entry_length);

void cargar_operadores(GtkWidget *entry[]);

GtkWidget *make_entry_with_label(GtkTable *table,
                     const gchar *label_text,
                     gint left_attach,
                     gint right_attach,
                     gint top_attach,
                     gint bottom_attach,
                     gint entry_length)
{

    GtkWidget *entry;
    GtkWidget *label;

    label = gtk_label_new(label_text);
    entry = gtk_entry_new();
    gtk_table_attach(table, label, left_attach, right_attach, top_attach, bottom_attach, GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND, 5, 5);
    gtk_table_attach(table, entry, left_attach + 1, right_attach + 1, top_attach, bottom_attach, GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND, 5, 5);
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_misc_set_padding(GTK_MISC(label), 2, 2);
    gtk_entry_set_max_length(GTK_ENTRY(entry), entry_length);

    gtk_widget_show(entry);
    gtk_widget_show(label);

    return entry;
}


void cargar_operadores(GtkWidget *entry[])
{
    
	toperador1 = gtk_entry_get_text(GTK_ENTRY(entry[0]));
	toperador2 = gtk_entry_get_text(GTK_ENTRY(entry[1]));
	resultado = gtk_entry_get_text(GTK_ENTRY(entry[2]));

	printf("Operador 1: %s \n",toperador1);
	printf("Operador 2: %s \n",toperador2);
	printf("Resultado: %s \n",resultado);

}



void procesar_letras()
{

	int i,j;
	char *temp_letras;/*array donde almaceno temporalmente todas las letras*/
	int k = 0;
	
	
	/*alojo memoria para el array temporal donde concateno los strings*/
	temp_letras = (char*)malloc(15*sizeof(char));

	/*pongo a cero temp_letras*/	
	memset(temp_letras,0,15);
	memset(array_letras,'_',10);

	/*concateno los 3 strings en 1*/
	strcat(temp_letras,toperador1);
	strcat(temp_letras,toperador2);
	strcat(temp_letras,resultado);

	printf("ARRAY TEMPORAL: %s\n",temp_letras);

	/*busco letras dentro del string que no se repiten*/
	

		for(i=0;i < strlen(temp_letras);i++)
		{

			if(temp_letras[i] != '_')
			{
		
				array_letras[k] = temp_letras[i];
				

				for(j=i+1;j < strlen(temp_letras);j++)
				{

					if(temp_letras[j] == array_letras[k])
					{

						temp_letras[j] = '_';
					
					} 					
					
				}
	
				k++;
				
			}
			
			if(k >= 10)break;


		}		
	
	printf("ARRAY TEMPORAL FILTRADO: %s\n",temp_letras);
	printf("LETRAS DEL ARRAY %s\n",array_letras);

	for(i=0;i < 10;i++)
	{

		if(array_letras[i] != '_')
		{
			cant_letras++;
		}

	}

	printf("LONGITUD ARRAY LETRAS QUE NO SE REPITEN: %d\n",cant_letras);


}

void iniciar_luciernagas(int num_luciernagas)
{

	int i,j;
	n_luc = num_luciernagas;

	luciernagas = (sfirefly*)malloc(n_luc * sizeof(sfirefly*));
	
	for(i=0;i < n_luc;i++)
	{

		/*pongo a cero la luciernaga*/
		memset(luciernagas[i].luc_numeros,0,10);
		luciernagas[i].luc_intensidad = 0;
		
		/*genero los numeros aleatorios*/
		generar_numeros(luciernagas[i].luc_numeros);

		/*calculo funcion objetivo*/
		luciernagas[i].luc_intensidad = calcular_func_objetivo(SUMA,luciernagas[i].luc_numeros);


	}



	for(i=0;i < n_luc;i++)
	{
		printf("LUCIERNAGA %d: ",i); 
		for(j=0;j<10;j++)printf("%d",luciernagas[i].luc_numeros[j]);
		printf("\n");
		printf("BRILLO: %f\n",luciernagas[i].luc_intensidad);

	}	

}


void correr_algoritmo(int numcorridas)
{

	int i,j,k;
	
	int t=0;

	int *distancia;

	while(t < numcorridas)
	{

		for(i=0;i < n_luc;i++)
		{
			for(j=0;j < n_luc;j++)
			{
				
				if((luciernagas[i].luc_intensidad) > (luciernagas[j].luc_intensidad))
				{
					
					printf("VECTOR %d: ",i);
					for(k=0;k < 10;k++)
					{					
						printf("%d ",luciernagas[i].luc_numeros[k]);
					}
					printf("\n");

					printf("VECTOR %d: ",j);
					for(k=0;k < 10;k++)
					{					
						printf("%d ",luciernagas[j].luc_numeros[k]);
					}
					printf("\n");

					/*calculo la distancia entre i y j*/
					distancia = calcular_distancia(luciernagas[i].luc_numeros,luciernagas[j].luc_numeros);
					
					printf("DISTANCIA: ");
					for(k=0;k < 10;k++)
					{					
						printf("%d ",distancia[k]);
					}
					printf("\n");

					/*tengo que acercar luciernaga i a j*/
					acercar_luciernaga(luciernagas[i].luc_numeros,luciernagas[j].luc_numeros,distancia);

					printf("VECTOR ACERCADO %d: ",j);
					for(k=0;k < 10;k++)
					{					
						printf("%d ",luciernagas[j].luc_numeros[k]);
					}
					printf("\n");

					/*recalculo funcion objetivo para j*/
					luciernagas[j].luc_intensidad = calcular_func_objetivo(SUMA,luciernagas[j].luc_numeros);
					printf("INTENSIDAD RECALCULADA %f \n: ",luciernagas[j].luc_intensidad);
					printf("\n");




				}


			}
		}
		
		/*ordeno la poblacion de luciernagas por brillo*/
		ordenar_luciernagas();
	
		t++;

	}

		printf("LUCIERNAGA 0:");
		for(i=0;i < 10;i++)
		{					
			printf("%d ",luciernagas[0].luc_numeros[i]);
			
		}
		printf("BRILLO 0: %f",luciernagas[0].luc_intensidad);
		printf("\n");


}

void ordenar_luciernagas()
{

	int i,j;

	sfirefly temp;

	for(i=0 ;i<( n_luc - 1 );i++)
	{
		for (j=0;j < n_luc - i - 1;j++)
		{
			if ((luciernagas[j].luc_intensidad) <= (luciernagas[j+1].luc_intensidad))
			{
				temp = luciernagas[j];
				luciernagas[j] = luciernagas[j+1];
				luciernagas[j+1] = temp;
			}
		}
	}

	/*for(i=0;i<n_luc;i++)
	{
		printf("LUCIERNAGA %d: %f\n",i,luciernagas[i].luc_intensidad);
	}*/





}

void acercar_luciernaga(int array_mayor[10],int array_menor[10],int distancia[10])
{

	int i;
	int mayor = distancia[0];
	int pos = 0;
	int randompos = 0;
	int temp = 0;

	/*busco la mayor distancia y su posicion dentro del array*/
	for(i=0;i < cant_letras;i++)
	{
		if(distancia[i] >= mayor)
		{
			mayor = distancia[i];
			pos = i;
		} 	
	}

	printf("MAYOR DISTANCIA: %d POSICION: %d",mayor,pos);
	printf("\n");

	/*busco valor random distinto de pos entre 0 y 9*/
	while(randompos == pos)randompos = rand() % 10;

	/*intercambio valores de las posiciones*/
	temp = array_menor[pos];
	array_menor[pos] = array_menor[randompos];
	array_menor[randompos] = temp;
		

}


int *calcular_distancia(int array1[10],int array2[10])
{

	int i;

	int valor = 0;

	static int array_temp[10];

	memset(array_temp,0,10);

	for(i=0;i < cant_letras;i++)
	{

		valor = abs(array1[i] - array2[i]);
		array_temp[i] = valor;
		//printf("%d\n",array_temp[i]);
	
	}

	return array_temp;

}


void generar_numeros(int array[10]) 
{

	int i;

	int in1 = 0;/*indice 1*/
	int in2 = 0;/*indice 2*/

	int numtemp[10] = {0,1,2,3,4,5,6,7,8,9};
	int num = 0;

	/*HAGO 10 PERMUTACIONES CON 2 POSICIONES ALEATORIAS DIFERENTES EN EL ARREGLO DE NUMEROS NUMTEMP*/
		
	for(i=0;i < 10;i++)
	{	
		
		in1 = rand() % 10;		
		while(in1 == in2)in2 = rand() % 10;/*hasta que no sea diferente a in1 genero numeros en in2*/	
	
		/*hago la permutacion*/

		num = numtemp[in1];
		numtemp[in1] = numtemp[in2];
		numtemp[in2] = num;
		num = 0;
	
	}

	/*for(i=0;i<10;i++)printf("%d",numtemp[i]);
	printf("\n");*/

	/*asigno el array generado*/
	memcpy(array,numtemp,sizeof(numtemp));

}

float calcular_func_objetivo(int operacion,int array_num[10])
{

	int op1 = 0;
	int op2 = 0;
	int op3 = 0;
	int ac = 0;
	float error = 0;

	/*calculo los 3 operadores*/
	op1 = calcular_operador(toperador1,array_num);
	op2 = calcular_operador(toperador2,array_num);	
	op3 = calcular_operador(resultado,array_num);

	/*printf("%d\n",op1);
	printf("%d\n",op2);
	printf("%d\n",op3);*/

	/*si la letra que corresponde al digito de mas a la izquierda fuera 0, la funcion calcular_operador devuelve un -1, 
	  por lo cual se penaliza el resultado de ac con un numero alto*/
	if((op1 == -1)||(op2 == -1)||(op3 == -1))
	{
		
		ac = pow(10,8);/*devuelve 10 a la 8*/

	}else{	

		/*de otra manera se calcula normalmente la funcion objetivo*/	
		if(operacion == SUMA)
		{
			ac = (unsigned)(op3 - (op1 + op2));		
		}

	}

	/*me fijo si tengo que evitar division por 0*/		
	if(ac == 0)
	{

		error = pow(10,8);

	}else{

		error = (1/(float)ac);

	}

	printf("%d\n",ac);

	printf("%f\n",error);

	return error;

}

int calcular_operador(const char *operador,int array_num[10])
{

	int i,j;
	int exponente = 0;
	int ac = 0;

	for(i=(strlen(operador)-1);i >= 0;i--)
	{

		for(j=0;j < strlen(array_letras);j++)
		{

			/*verifico que el digito de mas a la izquierda del operador sea distinto de 0*/
			if((operador[0] == array_letras[j]) && (array_num[j] == 0))
			{
				return -1;				
			} 

			
			if(operador[i] == array_letras[j])
			{


				ac = ac + (array_num[j] * pow(10,exponente));
		
				//printf("%d\n",ac);			
			
			}
		
		}						

		exponente++;

	}

	return ac;

}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
	
	GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *bbox;
    GtkWidget *boton;
    GtkWidget *entry[3];
    GtkWidget *frame;
    GtkWidget *table;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Firefly");
    gtk_widget_set_size_request(window, 350, 250);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(GTK_WINDOW(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);

    frame = gtk_frame_new("Ingrese operandos");
    vbox = gtk_vbox_new(FALSE, 5);
    table = gtk_table_new(4,2, FALSE);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), table);

    entry[0] = make_entry_with_label(GTK_TABLE(table), "Operador 1", 0,1,0,1,5);
    entry[1] = make_entry_with_label(GTK_TABLE(table), "Operador 2", 0,1,1,2,5);
    entry[2] = make_entry_with_label(GTK_TABLE(table), "Resultado", 0,1,2,3,5);
    
    bbox = gtk_hbutton_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), bbox, TRUE, TRUE, 3);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing(GTK_BOX(bbox), 10);

    boton = gtk_button_new_with_label("Calcular");
    gtk_container_add(GTK_CONTAINER(bbox), boton);
    g_signal_connect_swapped(G_OBJECT(boton), "clicked", G_CALLBACK(cargar_operadores), entry);
    boton = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
    g_signal_connect(G_OBJECT(boton), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_add(GTK_CONTAINER(bbox), boton);
    gtk_widget_show_all(window);
    gtk_main();

	procesar_letras();
	iniciar_luciernagas(100);
	correr_algoritmo(10);

    return 0;

}


