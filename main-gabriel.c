#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

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
int n_luc = 10;/*numero total de luciernagas*/
int n_it = 1;/*numero total de iteraciones*/

char array_letras[10];/*letras que no se repiten*/
int cant_letras;/*cantidad de letras que no se repiten*/

int operacion = 0;

double alfa = 0.5;		
double betaini = 1.0;           
double gama = 0.002;

float bri_ant = 0;/*brillo de la luciernaga 0 en la iteracion anterior*/
int cont_brillo = 0;/*cantidad de veces que la luciernaga 0 brille igual*/


typedef struct 
{

	int luc_numeros[10];
	float luc_intensidad;
	
}sfirefly;


sfirefly *luciernagas;


/*funciones*/
void generar_numeros(int array[10]);

float calcular_func_objetivo(int operacion,int array_num[10]);

float calcular_func_objetivo2(int operacion,int array_num[10]);

int calcular_operador(const char *operador,int array_num[10]);

int *calcular_distancia(int array1[10],int array2[10]);

void acercar_luciernaga(int array_mayor[10],int array_menor[10],int distancia[10]);

void acercar_luciernaga2(int array_mayor[10],int array_menor[10],int distancia[10]);

void acercar_luciernaga3(int array_mayor[10],int array_menor[10]);

void acercar_luciernaga4(int array_mayor[10],int array_menor[10]);

void ordenar_luciernagas();

void alejar_luciernaga(int array_numeros[10]);




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

	/*alojo memoria para la poblacion de luciernagas*/
	luciernagas = (sfirefly*)malloc(num_luciernagas * sizeof(sfirefly*));
	
	for(i=0;i < n_luc;i++)
	{

		/*pongo a cero la luciernaga*/
		memset(luciernagas[i].luc_numeros,0,10);
		luciernagas[i].luc_intensidad = 0;
		
		/*genero los numeros aleatorios*/
		generar_numeros(luciernagas[i].luc_numeros);

		/*calculo funcion objetivo*/
		luciernagas[i].luc_intensidad = calcular_func_objetivo2(operacion,luciernagas[i].luc_numeros);


	}



	/*for(i=0;i < n_luc;i++)
	{
		printf("LUCIERNAGA %d: ",i); 
		for(j=0;j<10;j++)printf("%d",luciernagas[i].luc_numeros[j]);
		printf("\n");
		printf("BRILLO: %f\n",luciernagas[i].luc_intensidad);

	}*/	

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
				
				if((luciernagas[i].luc_intensidad) < (luciernagas[j].luc_intensidad))
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
					//distancia = calcular_distancia(luciernagas[i].luc_numeros,luciernagas[j].luc_numeros);
					
					/*printf("DISTANCIA: ");
					for(k=0;k < 10;k++)
					{					
						printf("%d ",distancia[k]);
					}
					printf("\n");*/

					/*tengo que acercar luciernaga i a j*/
					//acercar_luciernaga2(luciernagas[i].luc_numeros,luciernagas[j].luc_numeros,distancia);
					acercar_luciernaga3(luciernagas[i].luc_numeros,luciernagas[j].luc_numeros);

					printf("VECTOR ACERCADO %d: ",j);
					for(k=0;k < 10;k++)
					{					
						printf("%d ",luciernagas[j].luc_numeros[k]);
					}
					printf("\n");

					/*recalculo funcion objetivo para j*/
					luciernagas[j].luc_intensidad = calcular_func_objetivo2(operacion,luciernagas[j].luc_numeros);
					printf("INTENSIDAD RECALCULADA %f \n: ",luciernagas[j].luc_intensidad);
					printf("\n");




				}


			}
		}
		
		/*ordeno la poblacion de luciernagas por brillo*/
		ordenar_luciernagas();

		printf("LUCIERNAGA 0 iteracion %d: ",t);
		for(i=0;i < 10;i++)
		{					
			printf("%d ",luciernagas[0].luc_numeros[i]);
			
		}
		printf("BRILLO 0: %f",luciernagas[0].luc_intensidad);
		printf("\n");

		if((bri_ant == luciernagas[0].luc_intensidad) && (luciernagas[0].luc_intensidad != 0))
		{
			cont_brillo++;
		}else{
			cont_brillo = 0;
		}

		if(cont_brillo == 5)
		{

			alejar_luciernaga(luciernagas[0].luc_numeros);

			luciernagas[0].luc_intensidad = calcular_func_objetivo2(operacion,luciernagas[0].luc_numeros);

			printf("LUCIERNAGA 0 iteracion %d cambiada: ",t);
			for(i=0;i < 10;i++)
			{					
				printf("%d ",luciernagas[0].luc_numeros[i]);
			
			}
			printf("BRILLO 0 cambiada: %f",luciernagas[0].luc_intensidad);
			printf("\n");

			cont_brillo = 0;

			/*ordeno la poblacion de luciernagas por brillo*/
			ordenar_luciernagas();

		}

		bri_ant = luciernagas[0].luc_intensidad;

		t++;

	}

		printf("LUCIERNAGA 0: ");
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
			if ((luciernagas[j].luc_intensidad) >= (luciernagas[j+1].luc_intensidad))
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

	/*printf("MAYOR DISTANCIA: %d POSICION: %d",mayor,pos);
	printf("\n");*/

	/*busco valor random distinto de pos entre 0 y 9*/
	while(randompos == pos)randompos = rand() % 10;

	/*intercambio valores de las posiciones*/
	temp = array_menor[pos];
	array_menor[pos] = array_menor[randompos];
	array_menor[randompos] = temp;
		

}

void acercar_luciernaga2(int array_mayor[10],int array_menor[10],int distancia[10])
{

	int i,j;
	int n = 10;
	int mayor = 0;
	int pmayor = 0;
	int segundo = 0;
	int psegundo =0;
	int temp = 0;
	int pos = 0;
	
	/*busco la mayor distancia y su posicion dentro del array*/
	for(i=0;i < cant_letras;i++)
	{
		if(distancia[i] >= mayor)
		{
			mayor = distancia[i];
			pmayor = i;
		} 	
	}

	/*busco la segunda mayor distancia dentro del array*/
	for(i=0;i < 10;i++)
	{
		if((distancia[i] >= segundo) && (i!=pmayor))
		{
			segundo = distancia[i];
			psegundo = i;
		} 	
	}

	/*printf("distancia: ");
	for(i=0;i<10;i++)
	{

		printf("%d",distancia[i]);

	}
	printf("\n");

	printf("mayor distancia:%d posicion:%d\n",mayor,pmayor);
	printf("2da mayor distancia:%d posicion:%d\n",segundo,psegundo);*/
	

	/*intercambio valores de las posiciones*/

	/*if(cant_letras < 10)
	{
		pos = (cant_letras -1) + (rand() % (10 - cant_letras -1));
		temp = array_menor[pmayor];
		array_menor[pmayor] = array_menor[pos];
		array_menor[pos] = temp;
		

	}else{
		temp = array_menor[pmayor];
		array_menor[pmayor] = array_menor[psegundo];
		array_menor[psegundo] = temp;
	}*/
	temp = array_menor[pmayor];
	array_menor[pmayor] = array_menor[psegundo];
	array_menor[psegundo] = temp;
		

}

void acercar_luciernaga3(int array_mayor[10],int array_menor[10])
{

	int i,j,k;

	int dist = 0;/*distancia total entre las 2 luciernagas*/

	int *distancia;/*arreglo de distancias entre luciernagas para cada una de las letras*/

	float beta = 0;

	int num_perm = 0;/*numero de permutaciones a realizar*/
	
	int mayor = 0;
	int pmayor = 0;
	int segundo = 0;
	int psegundo =0;
	int temp = 0;
	int pos = 0;

	float nrandom = 0;

	/*calculo distancia manhattan para cada letra*/
	distancia = calcular_distancia(array_mayor,array_menor);
	
	/*calculo distancia total*/
	for(i=0;i < cant_letras;i++)
	{

		dist = dist + distancia[i];

	}
	
	/*calculo beta*/
	beta = betaini * exp(-gama * pow(dist,2));

	nrandom = (float)rand()/(float)(RAND_MAX/1.0);

	num_perm = round(beta * dist) + (alfa * (nrandom - 0.5));

	//printf("%d\n",num_perm);

	
	for(i=0;i < num_perm;i++)
	{

		distancia = calcular_distancia(array_mayor,array_menor);


		for(j=0;j < cant_letras;j++)
		{
			if(distancia[j] >= mayor)
			{
				mayor = distancia[j];
				pmayor = j;
			} 	
		}


		for(k=0;k < 10;k++)
		{
			if((distancia[k] >= segundo) && (k!=pmayor))
			{
				segundo = distancia[k];
				psegundo = k;
			} 	
		}

		temp = array_menor[pmayor];
		array_menor[pmayor] = array_menor[psegundo];
		array_menor[psegundo] = temp;

	}

	
		

}

void acercar_luciernaga4(int array_mayor[10],int array_menor[10])
{

	int i,j;

	float nrandom = 0.0;

	int numero = 0;

	int *distancia;

	int dist = 0;

	float beta = 0;

	int flag = 0;

	//int *array_destino = (int*)malloc(10 * sizeof(int));

	static int array_destino[10];

	/*calculo distancia manhattan para cada letra*/
	distancia = calcular_distancia(array_mayor,array_menor);
	
	/*calculo distancia total*/
	for(i=0;i < cant_letras;i++)
	{

		dist = dist + distancia[i];

	}

	/*calculo beta*/
	beta = 1 / (1 + (gama*pow(dist,2)));

	printf("beta: %f\n",beta);


	/*- si son iguales los numeros en la misma posicion en array mayor y menor, lleno array destino con ese numero
	  - si random es menor a beta lleno posicion de array_destino con array menor, si no con array mayor
	  - si el numero ya se uso, marco posicion con un -1 y paso a la siguiente posicion */	

	for(i=0;i < 10;i++)
	{

		if(array_mayor[i] == array_menor[i])
		{

			array_destino[i] = array_mayor[i];

		}else{

			/*genera un numero random flotante*/
			nrandom = (float)rand()/(float)(RAND_MAX/1);

			printf("%f\n",nrandom);

			if(nrandom < beta)
			{
				numero = array_menor[i];
			}else if(nrandom >= beta){
				numero = array_mayor[i];
			}

			for(j=0;j < i;j++)
			{
				if(numero == array_destino[j])
				{
					flag = 1;
				}
			}

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
	
	/*
	for(i=0;i < 10;i++)
	{

		if(array_destino[i] == -1)
		{


			flag = 0;

			do{

				
	
			}while(flag == 0)

			array_destino[i] = numero;

		}

	}
	*/	
	
	/*asigno el array generado*/
	memcpy(array_menor,array_destino,sizeof(array_destino));
	

}


void alejar_luciernaga(int array_numeros[10])
{

	int valor1 = 0;
	int valor2 = 0;
	int posicion = 0;
		
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
			ac = abs(op3 - (op1 + op2));		
		}

	}

	/*me fijo si tengo que evitar division por 0*/		
	/*if(ac == 0)
	{

		error = pow(10,8);

	}else{

		error = (1/(float)ac);

	}

	printf("%d\n",ac);

	printf("%f\n",error);*/

	return ac;

}


/*prueba de funcion objetivo nueva*/
float calcular_func_objetivo2(int operacion,int array_num[10])
{

	int op1 = 0;
	int op2 = 0;
	int op3 = 0;
	int ac = 0;
	int i;
	float error = 0;
	float peso = 0;

	/*calculo los 3 operadores*/
	op1 = calcular_operador(toperador1,array_num);
	op2 = calcular_operador(toperador2,array_num);	
	op3 = calcular_operador(resultado,array_num);

	/*printf("Operador1: %d\n",op1);
	printf("operador2: %d\n",op2);
	printf("operador3: %d\n",op3);*/

	/*si alguno de los operadores tiene un 1 en el digito de mayor peso, lo penalizo, no puede ser solucion*/
	if((op1 == -1)||(op2 == -1)||(op3 == -1))
	{
		
		ac = pow(10,8);/*devuelve 10 a la 8*/

	}else{
		
		if(operacion == 0)
		{
			for(i=0;i < 5;i++)
			{
	
				//peso = 1/tanh(i);
				peso = pow(10,5-i);	
				ac = abs((((op1 + op2)%(int)pow(10,i+1)) - (op3%(int)pow(10,i+1)))) * peso;

			}
		}else if(operacion == 1){
	
			for(i=0;i < 5;i++)
			{
	
				//peso = 1/tanh(i);
				peso = pow(10,5-i);	
				ac = abs((((op2 + op3)%(int)pow(10,i+1)) - (op1%(int)pow(10,i+1)))) * peso;

			}

		}


	}
		

	

	/*me fijo si tengo que evitar division por 0*/		
	if(ac == 0)
	{

		error = pow(10,8);

	}else{

		error = (1/(float)ac);

	}

	//printf("%d\n",ac);

	//printf("%f\n",error);

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
			if((i==0) && (operador[i] == array_letras[j]) && (array_num[j] == 0))
			{
				return -1;				
			}

			
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
    	gtk_init(&argc, &argv);
	
		GtkWidget *window;
    	GtkWidget *vbox;
    	GtkWidget *bbox;
    	GtkWidget *boton;
    	GtkWidget *entry[3];
    	GtkWidget *frame;
    	GtkWidget *table;

		//operación: suma o resta
    	GtkWidget *foperaciones;
    	GtkWidget *toperaciones;
    	GtkWidget *rbox;
    	GtkWidget *rentry; 
    	GtkWidget *ropsuma;
    	GtkWidget *ropresta;

		//parametros
    	GtkWidget *fparametros;
    	GtkWidget *eparametros[5]; 
    	GtkWidget *tparametros;

    	GtkWidget *lparametros[5];
    	GtkWidget *aparametros[5]; 
    	GtkWidget *sparametros[5]; 
		//0 - cant de iteraciones
		//1 - cant de luciernagas
		//2 - alfa
		//3 - beta
		//4 - gamma


    	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    	gtk_window_set_title(GTK_WINDOW(window), "Firefly");
    	gtk_widget_set_size_request(window, 350, 600);
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

		//radio de operaciones
	   	foperaciones = gtk_frame_new("Tipo de Operación");
    	toperaciones = gtk_table_new(2,2, FALSE);
    	gtk_box_pack_start(GTK_BOX(vbox), foperaciones, FALSE, FALSE, 0);
    	gtk_container_add(GTK_CONTAINER(foperaciones), toperaciones);


//		rbox = gtk_vbox_new (TRUE, 1);
		ropsuma = gtk_radio_button_new_with_label(NULL, "Suma");
//		gtk_container_add (GTK_CONTAINER (ropsuma), rentry);
		ropresta = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (ropsuma),
				                    "Resta");

		gtk_table_attach_defaults(toperaciones, ropsuma,0 ,1, 0, 1);
		gtk_table_attach_defaults(toperaciones, ropresta,0 ,1, 1, 2);

//		gtk_box_pack_start(GTK_BOX (rbox), ropsuma, TRUE, TRUE, 1);
//		gtk_box_pack_start(GTK_BOX (rbox), ropresta, TRUE, TRUE, 1);
//		gtk_container_add(GTK_CONTAINER (window), rbox);
//    	gtk_box_pack_start(GTK_BOX(vbox), rbox, TRUE, TRUE, 3);


		//parametros
//		rentry = gtk_entry_new();
	   	fparametros = gtk_frame_new("Parámetros del algoritmo");
    	tparametros = gtk_table_new(5,2, FALSE);
    	gtk_box_pack_start(GTK_BOX(vbox), fparametros, FALSE, FALSE, 0);
    	gtk_container_add(GTK_CONTAINER(fparametros), tparametros);
	
		aparametros[0] = gtk_adjustment_new (10.0, 1, 1000.0, 1.0,5.0, 0.0); //cantidad de iteraciones
		aparametros[1] = gtk_adjustment_new (100.0, 1, 1000.0, 1.0,5.0, 0.0); //cantidad de luciernagas
		aparametros[2] = gtk_adjustment_new (0.5, 0, 1000.0, 0.001,0.5, 0.1); //alfa
		aparametros[3] = gtk_adjustment_new (0.1, 0, 1000.0, 0.001,0.5, 0.1); //beta - no se usa
		aparametros[4] = gtk_adjustment_new (0.002, 0, 1000.0, 0.001,0.5, 0.1); //gamma

		sparametros[0] = gtk_spin_button_new (aparametros[0], 0, 0);
		sparametros[1] = gtk_spin_button_new (aparametros[1], 0, 0);
		sparametros[2] = gtk_spin_button_new (aparametros[2], 0, 3);
		sparametros[3] = gtk_spin_button_new (aparametros[3], 0, 3);
		sparametros[4] = gtk_spin_button_new (aparametros[4], 0, 3);

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


	procesar_letras();
	iniciar_luciernagas(n_luc);
	correr_algoritmo(n_it);

    return 0;

}


