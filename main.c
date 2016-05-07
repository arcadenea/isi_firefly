#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

/*operadores*/
const char *toperador1;
const char *toperador2;
const char *resultado;

/*constantes*/
int n_luc;/*numero total de luciernagas*/
int n_gen = 1000;/*numero total de generaciones*/

char array_letras[10];/*letras que no se repiten*/
int cant_letras;/*cantidad de letras que no se repiten*/



struct nodo
{

	int luc_numeros[10];
	int luc_error;

	struct nodo *proximo; 
	
};

typedef struct nodo item;

item *curr,*head;


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
	printf("LONGITUD ARRAY LETRAS: %d\n",strlen(array_letras));


}

void iniciar_luciernagas(int num_luciernagas)
{

	int i;
	n_luc = num_luciernagas;

	head = NULL;

	for(i=0;i < n_luc;i++)
	{

		curr = (item *)malloc(sizeof(item));

		memset(curr->luc_numeros,0,10);
		curr->luc_error = 0;
		
		generar_numeros(curr->luc_numeros);

		curr->proximo = head;
		head = curr;

	}

	curr = head;

}

void generar_numeros(int array[10]) 
{

	int i,j;

	for(i=0;i < 10;i++)
	{	

		if(array[i] != '_')
		{	
			array[i] = rand() % 10;		
		}
	
		for(j=0;j < 10;j++)
		{

			if((array[i] == array[j]) && (i != j))
			{
					
				array[i] = rand() % 10;				

			}

		}
	
	}



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

    return 0;

}


