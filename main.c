#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>
#include "result.h" 

/*etiquetas para suma y resta*/
#define SUMA 0
#define RESTA 1


/*operadores*/
const char *toperador1;
const char *toperador2;
const char *resultado;

int longop1 = 10;/*longitud operador 1*/
int longop2 = 10;/*longitud operador 2*/
int longres = 10;/*longitud resultado*/

int runthreadidle=0; /*si es 1 indica que se debe correr el algoritmo*/

int t=0; /*variable de control para el numero de corridas*/

/*constantes*/
int n_luc = 200;/*numero total de luciernagas*/
int n_it = 10;/*numero total de iteraciones*/

char array_letras[10];/*letras que no se repiten*/
int cant_letras = 0;/*cantidad de letras que no se repiten*/

int operacion = SUMA;/*operacion a realizar*/

double alfa = 0.5;		
double betaini = 1.0;           
double gama = 0.1;

int bri_ant = 0;/*brillo de la luciernaga 0 en la iteracion anterior*/
int cont_brillo = 0;/*cantidad de veces que la luciernaga 0 brille igual*/

float prom_brillo = 0;/*promedio de brillo por iteracion*/
float total_prom_br = 0;/*promedio total de brillo por iteracion*/

GtkWidget *boton;
GtkWidget *botonpausa;
GtkWidget *botonstep;
GtkWidget *botonstop;
GtkWidget *botonayuda;

FILE *plotdatos;
//FILE *plotdatos2;

/*struct de luciernaga*/
typedef struct 
{

	int luc_numeros[10];/*array de numeros correspondientes a las letras sin repetir*/
	int luc_intensidad;/*intensidad de la luciernaga*/
	
}sfirefly;

/*población de luciernagas*/
sfirefly *luciernagas;


/*funciones*/
void generar_numeros(int array[10]);

int calcular_func_objetivo(int operacion,int array_num[10]);

int calcular_operador(const char *operador,int array_num[10]);

int *calcular_distancia(int array1[10],int array2[10]);

void acercar_luciernaga(int array_mayor[10],int array_menor[10],int distancia[10]);

void aplicar_alfa(int array[10]);

void ordenar_luciernagas();

void alejar_luciernaga(int array_numeros[10]);

void cargar_operadores(GtkWidget *entry[]);

GtkWidget *make_entry_with_label(GtkTable *table, const gchar *label_text, gint left_attach, gint right_attach, gint top_attach, 
                                gint bottom_attach, gint entry_length);


/* la siguiente funcion fue obtenida de la url:
   https://sourcecodebrowser.com/gbindadmin/0.1.5/make__settings__entries_8c.html
   del proyecto GBINDADMIN, a GTK+ frontend for ISC BIND
   Copyright C 2005, 2006 Magnus Loef <magnus-swe@telia.com> */

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


/*Muestra un mensaje en un cuadro de diálogo
obtenido de la documentacion contenida en https://developer.gnome.org/gtk2/stable/
*/
void mostrar_mensaje (char *message) {
	GtkWidget *dialog, *label, *content_area;

	dialog = gtk_dialog_new_with_buttons ("",
                                         NULL,
                                         GTK_DIALOG_MODAL,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_NONE,
                                         NULL);
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	label = gtk_label_new (message);

	g_signal_connect_swapped (dialog,
                             "response",
                             G_CALLBACK (gtk_widget_destroy),
                             dialog);

	gtk_container_add (GTK_CONTAINER (content_area), label);
	gtk_widget_show_all (dialog);
}

/*muestra mensaje de ayuda*/
void mostrar_ayuda()
{
mostrar_mensaje("Ingrese Operandos: En este área se ingresan los operandos con letras. Está compuesta de:\n\t- Operando 1: primer operando de la suma o resta.\n\t- Operando 2:  segundo operando de la suma o resta.\n\t- Resultado:  resultado de la suma o resta.\n\nTipo de operación: Operación algebraica a realizar. Puede ser Suma o Resta.\n\nParámetros del Algoritmo:\n\t- Cantidad de Iteraciones: número de veces que el algoritmo va a iterar.\n\t- Cantidad de Luciernagas: población total de luciernagas.\n\t- Alfa: valor del parámetro para el acercamiento alfa.\n\t- Gamma: valor del coeficiente de atracción.\n\nBotones de acción:\n\t- Calcular: ejecuta el algoritmo con parámetros establecidos previamente.\n\t- Paso: ejecuta iteración por iteración del algoritmo con parámetros establecidos previamente.\n\t- Pausa: pausar la ejecución del algoritmo. Para continuar la ejecución presionar Calcular o Paso.\n\t- Detener: detiene la ejecución del algoritmo.\nArea de resultados:\n\tI:Iteracion\n\tMB:Mejor Brillo\n\tBP:Brillo Promedio");
}


/* Función que se utiliza para correr el algoritmo sin bloquear a la GUI
obtenido de la documentacion contenida en https://developer.gnome.org/gtk2/stable/*/
static gboolean thread_func(void *algo)
{

	if(runthreadidle){
		gdk_threads_enter();
		//correr un paso del algoritmo aquí
		correr_algoritmo(n_it);
		gdk_threads_leave();
		g_usleep( 100 );
	}else{

		g_usleep( 1000 );
	}

	return( TRUE );

}

int chequear_textboxs(GtkWidget *entry[])
{

	int control=0;

	int tl1=gtk_entry_get_text_length(GTK_ENTRY(entry[0]));
	int tl2=gtk_entry_get_text_length(GTK_ENTRY(entry[1]));
	int tl3=gtk_entry_get_text_length(GTK_ENTRY(entry[2]));

	if((tl1==0)||(tl2==0)||(tl3==0))
	{
		mostrar_mensaje("Uno o varios operandos están vacíos");
		runthreadidle=0;
		t=0;
		return 1;
	}

	const char *op1=gtk_entry_get_text(GTK_ENTRY(entry[0]));
	const char *op2=gtk_entry_get_text(GTK_ENTRY(entry[1]));
	const char *res=gtk_entry_get_text(GTK_ENTRY(entry[2]));

	int i=0;

	for(i=0;i<tl1;i++)
	{
		if(!isalpha(op1[i]))
		{
			mostrar_mensaje("Los operandos y resultado deben ser caracteres de A-Z");
			control=1;
			break;
		}
	}

	for(i=0;i<tl2;i++)
	{
		if(!isalpha(op2[i]))
		{
			mostrar_mensaje("Los operandos y resultado deben ser caracteres de A-Z");
			control=1;
			break;		
		}
	}

	for(i=0;i<tl3;i++)
	{
		if(!isalpha(res[i]))
		{
			mostrar_mensaje("Los operandos y resultado deben ser caracteres de A-Z");
			control=1;
			break;
		}
	}

	return control;

}

/*obtenido de la documentacion contenida en https://developer.gnome.org/gtk2/stable/*/
void pausa_callback(GtkWidget *entry[])
{

	runthreadidle=0;
	gtk_widget_set_sensitive(GTK_WIDGET(boton),TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(botonstep),TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(botonstop),TRUE);

}

/*obtenido de la documentacion contenida en https://developer.gnome.org/gtk2/stable/*/
void detener_callback(GtkWidget *entry[])
{
	t=0;
	runthreadidle=0;
	gtk_widget_set_sensitive(GTK_WIDGET(boton),TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(botonstop),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(botonstep),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(botonpausa),FALSE);
}

/*obtenido de la documentacion contenida en https://developer.gnome.org/gtk2/stable/*/
void paso_callback(GtkWidget *entry[])
{
int control=0;
	if(t==0)
	{
		if(chequear_textboxs(entry)) return;
		cargar_operadores(entry);
	}


	correr_algoritmo(n_it);

}

/*obtenido de la documentacion contenida en https://developer.gnome.org/gtk2/stable/*/
void correr_callback(GtkWidget *entry[])
{

	int control=0;
	if(t==0)
	{
		if(chequear_textboxs(entry)) return;
		cargar_operadores(entry);
	}


	runthreadidle=1;

	gtk_widget_set_sensitive(GTK_WIDGET(boton),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(botonstop),TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(botonpausa),TRUE);
}


void cargar_operadores(GtkWidget *entry[])
{
	
	cant_letras=0;  

	toperador1 = gtk_entry_get_text(GTK_ENTRY(entry[0]));
	toperador2 = gtk_entry_get_text(GTK_ENTRY(entry[1]));
	resultado = gtk_entry_get_text(GTK_ENTRY(entry[2]));

	//PARTE NUEVA
	operacion = gtk_toggle_button_get_active(entry[4]); //operacion: suma o resta

	if(operacion == SUMA)
	{
		imprimir_salida("OPERACION: SUMA\n");
	}else{
		imprimir_salida("OPERACION: RESTA\n");
	}

	n_it = gtk_adjustment_get_value(entry[5]); //cantidad de iteraciones
	n_luc = gtk_adjustment_get_value(entry[6]); //cantidad de luciernagas
	alfa = gtk_adjustment_get_value(entry[7]); //alfa
	gama = gtk_adjustment_get_value(entry[9]); //gamma

	plotdatos = fopen("plotdatos.dat","w");

	procesar_letras();
	iniciar_luciernagas(n_luc);

	mostrar_ventana_resultados();

	imprimir_salida("I:Iteracion\nMB:Mejor Brillo\nBP:Brillo Promedio\n");
	imprimir_salida("I	MB	BP\n");
	

	//FIN PARTE NUEVA

}



void procesar_letras()
{

	int i,j;
	char *temp_letras;/*array donde almaceno temporalmente todas las letras*/
	int k = 0;
	int flag = 0;
		
	/*longitud del array temporal*/
	int tempnum = 0;
	tempnum = longop1 + longop2 + longres;

	/*alojo memoria para el array temporal donde concateno los strings*/
	temp_letras = (char*)malloc(tempnum * sizeof(char));
	
	/*pongo a cero temp_letras*/	
	memset(temp_letras,0,31);
	memset(array_letras,'_',10);

	/*concateno los 3 strings en 1*/
	strcat(temp_letras,toperador1);
	strcat(temp_letras,toperador2);
	strcat(temp_letras,resultado);

	/*busco letras dentro del string que no se repiten*/
	

		for(i=0;i < strlen(temp_letras);i++)
		{

			if(temp_letras[i] != '_')
			{
		
				if(k > 9)
				{
					imprimir_salida("solucion no factible\n");
					mostrar_mensaje("solución no factible");
					flag = 1;
					break;
				}	

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
			
			if(flag == 1)break;


		}		
	

	/*calculo cantidad de letras utilizadas*/
	for(i=0;i < 10;i++)
	{

		if(array_letras[i] != '_')
		{
			cant_letras++;
		}

	}


	free(temp_letras);

}

void iniciar_luciernagas(int num_luciernagas)
{

	/*variable de control*/
	int i;

	/*alojo memoria para la poblacion de luciernagas*/
	luciernagas = (sfirefly*)malloc(num_luciernagas * sizeof(sfirefly));
	
	for(i=0;i < n_luc;i++)
	{

		/*pongo a cero la luciernaga*/
		memset(luciernagas[i].luc_numeros,0,10);
		luciernagas[i].luc_intensidad = 0;
		
		/*genero los numeros aleatorios*/
		generar_numeros(luciernagas[i].luc_numeros);

		/*calculo funcion objetivo*/
		luciernagas[i].luc_intensidad = calcular_func_objetivo(operacion,luciernagas[i].luc_numeros);


	}


}


void correr_algoritmo(int numiteraciones)
{

	/*variables de control*/
	int i,j,k;
	

	/*variables donde se guardan los operandos para mostrarlos en la salida*/
	int op1,op2,op3;



	/*algoritmo firefly*/
	if(t < numiteraciones)
	{

		for(i=0;i < n_luc;i++)
		{
			for(j=0;j < n_luc;j++)
			{
				
				if((luciernagas[i].luc_intensidad) < (luciernagas[j].luc_intensidad))
				{

					int *distancia;					

					/*calculo distancia manhattan para cada letra*/
					distancia = calcular_distancia(luciernagas[i].luc_numeros,luciernagas[j].luc_numeros);

					/*acercamiento beta*/
					acercar_luciernaga(luciernagas[i].luc_numeros,luciernagas[j].luc_numeros,distancia);

					/*acercamiento alfa sobre luciernaga de menor brillo*/
					aplicar_alfa(luciernagas[j].luc_numeros);

					/*recalculo funcion objetivo*/
					luciernagas[j].luc_intensidad = calcular_func_objetivo(operacion,luciernagas[j].luc_numeros);

					free(distancia);

				}


			}
		}
		
		/*ordeno la poblacion de luciernagas por brillo*/
		ordenar_luciernagas();
	
		/*si el brillo actual de la luciernaga de mayor brillo es igual a brillo anterior, aumenta contador*/
		if((bri_ant == luciernagas[0].luc_intensidad) && (luciernagas[0].luc_intensidad != 0))
		{
			cont_brillo++;
		}else{
			cont_brillo = 0;
		}

		/*si contador de brillo es igual a 5, se llama a la funcion de alejar la luciernaga de mayor brillo*/
		if(cont_brillo == 5)
		{

			alejar_luciernaga(luciernagas[0].luc_numeros);

			luciernagas[0].luc_intensidad = calcular_func_objetivo(operacion,luciernagas[0].luc_numeros);
	
			cont_brillo = 0;

			ordenar_luciernagas();

			
		}

		

		/*se almacena sumatoria de brillos por iteracion*/
		for(i=0;i < n_luc;i++)
		{

			prom_brillo = (float)prom_brillo + (float)luciernagas[i].luc_intensidad;
		
		}

			/*se calcula el valor promedio de brillo por iteracion*/
			prom_brillo = (float)prom_brillo/(float)n_luc;

			fprintf(plotdatos,"%d	%d	%f\n",t,luciernagas[0].luc_intensidad, prom_brillo);

			imprimir_salida("%d	%d	%f\n",t,luciernagas[0].luc_intensidad,prom_brillo);
	
			/*se calcula brillo total promedio*/
			total_prom_br = total_prom_br + prom_brillo;
			prom_brillo = 0.0;	

		/*se resguarda el valor de brillo anterior*/
		bri_ant = luciernagas[0].luc_intensidad;

		t++;

	}else{

			runthreadidle=0;
			/*impresion de resultados finales*/
			imprimir_salida("\n");
			imprimir_salida("LETRAS DEL ARRAY %s\n",array_letras);
			imprimir_salida("LONGITUD ARRAY LETRAS QUE NO SE REPITEN: %d\n",cant_letras);
			imprimir_salida("RESULTADO FINAL: "); 
			for(i=0;i < 10;i++)
			{					

				imprimir_salida("%c=%d ",array_letras[i],luciernagas[0].luc_numeros[i]); 
			
			}
			imprimir_salida("\n");
			imprimir_salida("BRILLO FINAL: %d",luciernagas[0].luc_intensidad); 
			imprimir_salida("\n"); 


			op1 = calcular_operador(toperador1,luciernagas[0].luc_numeros);
			op2 = calcular_operador(toperador2,luciernagas[0].luc_numeros);	
			op3 = calcular_operador(resultado,luciernagas[0].luc_numeros);

			if(operacion == SUMA)
			{
				imprimir_salida("%s + %s = %s\n",toperador1,toperador2,resultado);
				imprimir_salida("%d + %d = %d\n",op1,op2,op3);
			}else{
				imprimir_salida("%s - %s = %s\n",toperador1,toperador2,resultado);
				imprimir_salida("%d - %d = %d\n",op1,op2,op3);
			}



			imprimir_salida("====================================================\n");

			/*calculo del brillo total promedio por iteraciones*/
			total_prom_br = total_prom_br/(float)numiteraciones;

			fclose(plotdatos);


			/*llamado a gnuplot para graficar*/
			system("gnuplot5 -persistent script.scp");


		
			/*se liberan punteros y se ponen variables al estado inicial*/
			free(luciernagas);
			t=0;
			gtk_widget_set_sensitive(GTK_WIDGET(boton),TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(botonstop),FALSE);
			gtk_widget_set_sensitive(GTK_WIDGET(botonstep),TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(botonpausa),FALSE);
		}



}

/*
	La implementacion del ordenamiento bubble sort
	se obtuvo de la siguiente url:
	http://www.programmingsimplified.com/c/source-code/c-program-bubble-sort
*/

void ordenar_luciernagas()
{

	/*variables de control*/
	int i,j;

	/*estructura temporal para almacenar una luciernaga*/
	sfirefly temp;
	
	/*algoritmo de ordenamiento bubble sort ascendente*/
	for(i=0 ;i<( n_luc - 1 );i++)
	{
		for (j=0;j < n_luc - i - 1;j++)
		{
			if ((luciernagas[j].luc_intensidad) >= (luciernagas[j+1].luc_intensidad))
			{
				temp = luciernagas[j];
				luciernagas[j] = luciernagas[j+1];
				luciernagas[j+1] = temp;
			}
		}
	}

}



void acercar_luciernaga(int array_mayor[10],int array_menor[10],int distancia[10])
{

	int i,j;

	float nrandom = 0.0;/*numero aleatorio flotante*/

	int nrandint = 0;/*numero aleatorio entero*/

	int numero = 0;/*resguardo de valor a llenar en posicion*/

	int dist = 0;/*distancia sumatoria de las distancias individuales de cada letra*/

	float beta = 0;/*valor de beta a calcular*/

	int flag = 0;

	int flag2 = 0;

	int array_destino[10];/*array temporal destino del acercamiento*/
	
	/*- si son iguales los numeros en la misma posicion en array mayor y menor, lleno array destino con ese numero
	  - si random es menor a beta lleno posicion de array_destino con array mayor, si no con array menor
	  - si el numero ya se uso, marco posicion con un -1 y paso a la siguiente posicion */	

	for(i=0;i < 10;i++)
	{

		/*si los valores de las 2 luciernagas en la posicion son iguales, se asigna array destino con ese valor*/
		if(distancia[i] == 0)
		{

			array_destino[i] = array_mayor[i];

		}else if(distancia[i] > 0){

			/*genera un numero aleatorio flotante*/
			nrandom = (float)rand()/(float)(RAND_MAX/1);

			beta = 1 / (1 + (gama * pow(distancia[i],2)));

			/*si el numero aleatorio es menor al beta, se asigna el valor de la luciernaga de mayor brillo,
			  sino se asigna el valor de la luciernaga de menor brillo.
			*/
			if(nrandom < beta)
			{
				numero = array_mayor[i];
			}else if(nrandom >= beta){
				numero = array_menor[i];
			}

			/*verifica que el valor de numero no haya sido utilizado anteriormente*/
			for(j=0;j < i;j++)
			{
				if(numero == array_destino[j])
				{
					flag = 1;
				}
			}

			/*si no fue usado el valor, se asigna ese numero. Sino, se marca con un -1 la posicion*/
			if(flag == 0)
			{
				array_destino[i] = numero;
			}else{	
				array_destino[i] = -1;
			}

			flag = 0;
			nrandom = 0;			
			
		}

	}


	/*al finalizar el llenado, van a quedar posiciones sin llenar, marcadas con -1
	  Procedo a llenarlas aleatoriamente con los valores que sobraron*/	
	for(i=0;i < 10;i++)
	{

		if(array_destino[i] == -1)
		{


			flag2 = 0;

			do{

				nrandint = rand() % 10;

				for(j=0;j < 10;j++)
				{
		
					if(nrandint == array_destino[j])
					{
						break;
					}	
	
				}

				if(j == 10) flag2 = 1;
	
			}while(flag2 == 0);

			array_destino[i] = nrandint;

		}

	}

	
	
	/*asigno el array generado*/
	memcpy(array_menor,array_destino,sizeof(array_destino));
	

}

void aplicar_alfa(int array[10])
{

	int nrandom = rand() % 10;/*numero aleatorio 1*/
	int nrandom2 = 0;/*numero aleatorio 2*/

	/*calculo del ajuste alfa*/
	int num = round(alfa * (nrandom - 0.5));

	int i;/*variable de control*/

	int temp = 0;/*variable de resguardo*/


	/*num es la cantidad de permutaciones que se van a realizar entre 2 posiciones aleatorias*/	
	for(i=0;i < num;i++)
	{

		/*genero primer posicion aleatoria*/
		nrandom = rand() % 10;
		
		/*busco que las 2 posiciones aleatorias sean diferentes*/	
		while(nrandom2 == nrandom) nrandom2 = rand() % 10;

		/*intercambio valores de las posiciones*/
		temp = array[nrandom];
		array[nrandom] = array[nrandom2];
		array[nrandom2] = temp;

	}
	



}


void alejar_luciernaga(int array_numeros[10])
{

	int valor1 = 0;
	int valor2 = 0;
	int posicion = 0;

	/*se busca que la posicion sea distinta del valor que contiene la posicion*/		
	do{
		
		posicion = rand() % cant_letras;
		valor1 = array_numeros[posicion];

	}while(posicion == valor1);	

	
	valor2 =  array_numeros[valor1];

	array_numeros[posicion] = valor2;
	array_numeros[valor1] = valor1;

}



int *calcular_distancia(int array1[10],int array2[10])
{

	int i;

	int valor = 0;

	int *array_temp = (int*)malloc(10*sizeof(int));

	memset(array_temp,0,10);

	for(i=0;i < 10;i++)
	{

		valor = abs(array1[i] - array2[i]);
		array_temp[i] = valor;
		//printf("%d ",valor);
	
	}	
	//printf("\n");

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

	/*asigno el array generado*/
	memcpy(array,numtemp,sizeof(numtemp));

}

int calcular_func_objetivo(int operacion,int array_num[10])
{

	/*operadores*/
	int op1 = 0;
	int op2 = 0;
	int op3 = 0;
	
	/*resultado funcion objetivo*/
	int ac = 0;

	/*calculo los 3 operadores*/
	op1 = calcular_operador(toperador1,array_num);
	op2 = calcular_operador(toperador2,array_num);	
	op3 = calcular_operador(resultado,array_num);

	
	/*si la letra que corresponde al digito de mas a la izquierda fuera 0, la funcion calcular_operador devuelve un -1, 
	  por lo cual se penaliza el resultado de ac con un numero alto*/
	if((op1 == -1)||(op2 == -1)||(op3 == -1))
	{
		
		//ac = pow(10,5);/*devuelve 10 a la 5*/

	}else{	

		/*de otra manera se calcula normalmente la funcion objetivo*/	
		if(operacion == SUMA)
		{
			
			ac = abs(op3 - (op1 + op2));		
		
		}else if(operacion == RESTA){

			ac = abs(op1 - (op2 + op3));

		}

	}

	/*devuelvo resultado*/
	return ac;

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
			/*if((i==0) && (operador[i] == array_letras[j]) && (array_num[j] == 0))
			{
				return -1;				
			}*/

			
			if(operador[i] == array_letras[j])
			{


				ac = ac + (array_num[j] * pow(10,exponente));
		
			
			}
		
		}						

		exponente++;

	}


	return ac;

}

int main(int argc, char *argv[])
{

	if(argc == 1)
	{

		/*codigo de la gui de GTK2+, licencia LGPL 2.1*/
    	gtk_init(&argc, &argv);
	
		GtkWidget *window;
    	GtkWidget *vbox;
    	GtkWidget *bbox;
//    	GtkWidget *boton;
    	GtkWidget *entry[10]; //PARTE NUEVA: CAMBIAR POR 10
    	GtkWidget *frame;
    	GtkWidget *table;

		//PARTE NUEVA
		//operación: suma o resta
    	GtkWidget *foperaciones;
    	GtkWidget *toperaciones;
    	GtkWidget *rbox;
    	GtkWidget *rentry; 
    	GtkWidget *ropsuma;
    	GtkWidget *ropresta;

		//parametros
    	GtkWidget *fparametros;
    	GtkWidget *tparametros;

    	GtkWidget *lparametros[5]; //labels
    	GtkWidget *sparametros[5]; //spin buttons
		//0 - cant de iteraciones
		//1 - cant de luciernagas
		//2 - alfa
		//3 - beta
		//4 - gamma
		//FIN PARTE NUEVA

    	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    	gtk_window_set_title(GTK_WINDOW(window), "Firefly");
    	gtk_widget_set_size_request(window, 500, 500);
		gtk_window_set_resizable(window, FALSE);
    	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    	g_signal_connect(GTK_WINDOW(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);

    	frame = gtk_frame_new("Ingrese operandos");
    	vbox = gtk_vbox_new(FALSE, 5);
    	table = gtk_table_new(4,2, FALSE);

    	gtk_container_add(GTK_CONTAINER(window), vbox);
    	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
    	gtk_container_add(GTK_CONTAINER(frame), table);

    	entry[0] = make_entry_with_label(GTK_TABLE(table), "Operando 1", 0,1,0,1,longop1);
    	entry[1] = make_entry_with_label(GTK_TABLE(table), "Operando 2", 0,1,1,2,longop2);
    	entry[2] = make_entry_with_label(GTK_TABLE(table), "Resultado", 0,1,2,3,longres);
    

		//PARTE NUEVA
		//radio buttons de operaciones
	   	foperaciones = gtk_frame_new("Tipo de Operación");
    	toperaciones = gtk_table_new(2,2, FALSE);
    	gtk_box_pack_start(GTK_BOX(vbox), foperaciones, FALSE, FALSE, 0);
    	gtk_container_add(GTK_CONTAINER(foperaciones), toperaciones);

		entry[3] = gtk_radio_button_new_with_label(NULL, "Suma");
		entry[4] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (entry[3]),
				                    "Resta");

		gtk_table_attach_defaults(toperaciones, entry[3],0 ,1, 0, 1);
		gtk_table_attach_defaults(toperaciones, entry[4],0 ,1, 1, 2);

		//parametros
	   	fparametros = gtk_frame_new("Parámetros del algoritmo");
    	tparametros = gtk_table_new(5,2, FALSE);
    	gtk_box_pack_start(GTK_BOX(vbox), fparametros, FALSE, FALSE, 0);
    	gtk_container_add(GTK_CONTAINER(fparametros), tparametros);
	
		entry[5] = gtk_adjustment_new (100.0, 1, 100000.0, 1.0,5.0, 0.0); //cantidad de iteraciones
		entry[6] = gtk_adjustment_new (1000.0, 1, 100000.0, 1.0,5.0, 0.0); //cantidad de luciernagas
		entry[7] = gtk_adjustment_new (0.5, 0, 1000.0, 0.001,0.5, 0.1); //alfa
		entry[8] = gtk_adjustment_new (0.1, 0, 1000.0, 0.001,0.5, 0.1); //beta - no se usa
		entry[9] = gtk_adjustment_new (0.1, 0, 1000.0, 0.001,0.5, 0.1); //gamma

		sparametros[0] = gtk_spin_button_new (entry[5], 0, 0);
		sparametros[1] = gtk_spin_button_new (entry[6], 0, 0);
		sparametros[2] = gtk_spin_button_new (entry[7], 0, 3);
		sparametros[3] = gtk_spin_button_new (entry[8], 0, 3);
		sparametros[4] = gtk_spin_button_new (entry[9], 0, 3);

		lparametros[0] = gtk_label_new("Cantidad de Iteraciones");
		lparametros[1] = gtk_label_new("Cantidad de Luciernagas");
		lparametros[2] = gtk_label_new("Alfa");
		lparametros[3] = gtk_label_new("Beta");
		lparametros[4] = gtk_label_new("Gamma");

		gtk_table_attach_defaults(tparametros, lparametros[0],0 ,1, 0, 1);
		gtk_table_attach_defaults(tparametros, lparametros[1],0 ,1, 1, 2);
		gtk_table_attach_defaults(tparametros, lparametros[2],0 ,1, 2, 3);
//		gtk_table_attach_defaults(tparametros, lparametros[3],0 ,1, 3, 4);
		gtk_table_attach_defaults(tparametros, lparametros[4],0 ,1, 3, 4);

		gtk_table_attach_defaults(tparametros, sparametros[0],1 ,2, 0, 1);
		gtk_table_attach_defaults(tparametros, sparametros[1],1 ,2, 1, 2);
		gtk_table_attach_defaults(tparametros, sparametros[2],1 ,2, 2, 3);
//		gtk_table_attach_defaults(tparametros, sparametros[3],1 ,2, 3, 4);
		gtk_table_attach_defaults(tparametros, sparametros[4],1 ,2, 3, 4);
		//FIN PARTE NUEVA

    	bbox = gtk_hbutton_box_new();
    	gtk_box_pack_start(GTK_BOX(vbox), bbox, TRUE, TRUE, 3);
    	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_CENTER);
    	gtk_box_set_spacing(GTK_BOX(bbox), 10);

    	boton = gtk_button_new_with_label("Calcular");
    	botonstep = gtk_button_new_with_label("Paso");
    	botonpausa = gtk_button_new_with_label("Pausa");
    	botonstop = gtk_button_new_with_label("Detener");
    	botonayuda = gtk_button_new_with_label("Ayuda");
    	gtk_container_add(GTK_CONTAINER(bbox), boton);
    	gtk_container_add(GTK_CONTAINER(bbox), botonstep);
    	gtk_container_add(GTK_CONTAINER(bbox), botonpausa);
    	gtk_container_add(GTK_CONTAINER(bbox), botonstop);
    	gtk_container_add(GTK_CONTAINER(bbox), botonayuda);
    	g_signal_connect_swapped(G_OBJECT(boton), "clicked", G_CALLBACK(correr_callback), entry);
    	g_signal_connect_swapped(G_OBJECT(botonstep), "clicked", G_CALLBACK(paso_callback), entry);
    	g_signal_connect_swapped(G_OBJECT(botonpausa), "clicked", G_CALLBACK(pausa_callback), entry);
    	g_signal_connect_swapped(G_OBJECT(botonstop), "clicked", G_CALLBACK(detener_callback), entry);
    	g_signal_connect_swapped(G_OBJECT(botonayuda), "clicked", G_CALLBACK(mostrar_ayuda), entry);
    	gtk_widget_show_all(window);
		mostrar_resultados(); //PARTE NUEVA

		gtk_widget_set_sensitive(GTK_WIDGET(botonpausa),FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(botonstop),FALSE);

		/*Correr hilo*/
		g_idle_add( thread_func, NULL );

		gtk_main ();

		gdk_threads_leave();

	}else{

		if((strlen(argv[1]) <= 5) && (strlen(argv[2]) <= 5) && (strlen(argv[3]) <= 5))
		{

			toperador1 = argv[1];
			toperador2 = argv[2];		
			resultado = argv[3];
			operacion =  atoi(argv[4]);

			printf("%s %s %s\n",toperador1,toperador2,resultado);
			printf("%d %d %d\n",strlen(toperador1),strlen(toperador2),strlen(resultado));
			printf("operacion: %d\n",operacion);

		}else{

			printf("uno de los operandos tiene mas de 5 caracteres\n");
			return 0;

		}
		

		
		

	}

    return 0;

}


