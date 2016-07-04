#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

GtkTextBuffer *buffer;
GtkWidget *window;
GtkWidget *swindow;

/*muestra ventana de resultados
obtenido de la documentacion contenida en https://developer.gnome.org/gtk2/stable/*/
void mostrar_ventana_resultados()
{
	GtkTextIter iteri;
	GtkTextIter iterf;
	gtk_widget_show_all(window);
	gtk_text_buffer_get_start_iter(buffer, &iteri); 
	gtk_text_buffer_get_end_iter(buffer, &iterf); 
	gtk_text_buffer_delete(buffer, &iteri, &iterf);

}

void mostrar_resultados()
{
		
		/*
			codigo de textview de GTK2+, licencia LGPL v2.1
			obtenido de la documentacion contenida en https://developer.gnome.org/gtk2/stable/
		*/

		printf("ventana nueva\n");

		GtkWidget *vsalida;
		GtkWidget *box;
		GtkWidget* table;

	   	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_resizable(window, FALSE);
    	gtk_window_set_title(GTK_WINDOW(window), "Resultados");
    	gtk_widget_set_size_request(window, 700, 600);
    	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	   	g_signal_connect(GTK_WINDOW(window), "delete_event", G_CALLBACK(gtk_widget_hide_all), window);

		swindow=gtk_scrolled_window_new(NULL,NULL);
		gtk_scrolled_window_set_policy(swindow, GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    	box = gtk_vbox_new(FALSE, 5);
		table = gtk_table_new(1, 1, FALSE);

		buffer=gtk_text_buffer_new(NULL);
		vsalida=gtk_text_view_new_with_buffer(buffer);

		//seteamos parámetros del textview
		gtk_text_view_set_editable((GtkTextView *) vsalida, 0);
		gtk_text_view_set_cursor_visible((GtkTextView *) vsalida, 0);



    	//gtk_container_add(GTK_CONTAINER(box), vsalida);
		gtk_container_add(GTK_CONTAINER(swindow), vsalida);
		gtk_table_attach_defaults(GTK_TABLE(table), swindow, 0, 1, 0, 1);

    	gtk_container_add(GTK_CONTAINER(window), table);
//    	gtk_widget_show_all(window);

//    	gtk_main();
}



void imprimir_salida(const char *format, ...)
{

	/* codigo basado en el printf de c, obtenido de
   	   http://stackoverflow.com/questions/4867229/code-for-printf-function-in-c
   	   adaptado a la libreria GTK2+, licencia LGPL 2.1
       codigo obtenido de la documentacion contenida en https://developer.gnome.org/gtk2/stable/
	*/
		va_list arg;
		int done;
		char salida[256];
		GtkTextIter iter;

		va_start (arg, format);
		vsprintf (salida, format, arg); //salida a un string
//		vfprintf (stdout, format, arg); //salida por stdout
		va_end (arg);

		//ponemos el cursor al final del buffer
		gtk_text_buffer_get_end_iter(buffer, &iter); 
		gtk_text_buffer_place_cursor(buffer, &iter); 

		//imprimimos la salida
		gtk_text_buffer_insert_at_cursor(buffer, salida, strlen(salida));

		GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(swindow);
		gtk_adjustment_set_value(adj,gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));


}
