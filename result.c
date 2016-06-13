#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

GtkTextBuffer *buffer;


void mostrar_resultados()
{
		
		printf("ventana nueva\n");
		GtkWidget *window;
		GtkWidget *vsalida;
		GtkWidget *box;

	   	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    	gtk_window_set_title(GTK_WINDOW(window), "Resultados");
    	gtk_widget_set_size_request(window, 350, 600);
    	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    	g_signal_connect(GTK_WINDOW(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);

    	box = gtk_vbox_new(FALSE, 5);

		buffer=gtk_text_buffer_new(NULL);
		vsalida=gtk_text_view_new_with_buffer(buffer);

		//seteamos parámetros del textview
		gtk_text_view_set_editable((GtkTextView *) vsalida, 0);
		gtk_text_view_set_cursor_visible((GtkTextView *) vsalida, 0);



    	gtk_container_add(GTK_CONTAINER(box), vsalida);


    	gtk_container_add(GTK_CONTAINER(window), box);
    	gtk_widget_show_all(window);
//    	gtk_main();
}

void imprimir_salida(const char *format, ...)
{
		va_list arg;
		int done;
		char salida[256];
		GtkTextIter iter;

		va_start (arg, format);
		vsprintf (salida, format, arg); //salida a un string
//		vfprintf (stdout, format, arg); //salida por stdout
		va_end (arg);

		//ponemos el cursor al final del buffer
		gtk_text_buffer_get_end_iter(buffer, &iter); //Get the end of the buffer
		gtk_text_buffer_place_cursor(buffer, &iter); //set the cursor at the end of the buffer

		//imprimimos la salida
		gtk_text_buffer_insert_at_cursor(buffer, salida, strlen(salida));


}
