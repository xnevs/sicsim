#include <gtk/gtk.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "Machine.h"
#include "Code.h"

struct Machine machine;

struct RegisterData {
	GtkEntryBuffer *PC;
	GtkEntryBuffer *SW;
	GtkEntryBuffer *A;
	GtkEntryBuffer *X;
	GtkEntryBuffer *L;
	GtkEntryBuffer *B;
	GtkEntryBuffer *S;
	GtkEntryBuffer *T;
	GtkEntryBuffer *F;
} register_data;

GtkEntryBuffer *frequency_data;

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
	return FALSE;
}
static void destroy(GtkWidget *widget, gpointer data) {
	gtk_main_quit();
}

static void update_register_data() {
	char str[256];
	sprintf(str, "0x%06lX", machine.PC);
	gtk_entry_buffer_set_text(register_data.PC, str, -1);
	sprintf(str, "0x%06lX", machine.SW);
	gtk_entry_buffer_set_text(register_data.SW, str, -1);
	sprintf(str, "0x%06lX", machine.A);
	gtk_entry_buffer_set_text(register_data.A, str, -1);
	sprintf(str, "0x%06lX", machine.X);
	gtk_entry_buffer_set_text(register_data.X, str, -1);
	sprintf(str, "0x%06lX", machine.L);
	gtk_entry_buffer_set_text(register_data.L, str, -1);
	sprintf(str, "0x%06lX", machine.B);
	gtk_entry_buffer_set_text(register_data.B, str, -1);
	sprintf(str, "0x%06lX", machine.S);
	gtk_entry_buffer_set_text(register_data.S, str, -1);
	sprintf(str, "0x%06lX", machine.T);
	gtk_entry_buffer_set_text(register_data.T, str, -1);
	sprintf(str, "%lf", machine.F);
	gtk_entry_buffer_set_text(register_data.F, str, -1);
}

static void update_frequency_data() {
	char str[256];
	sprintf(str, "%d", machine.freq);
	gtk_entry_buffer_set_text(frequency_data, str, -1);
}

int run = 0;
int running = 0;

long crnt_mem_view_addr = 0;
long crnt_mem_sele_addr = 0;
GtkTextBuffer *mem_buffer;

int mem_buffer_page_size = 22;
static void update_mem() {
	GtkTextIter start, end;

	gtk_text_buffer_get_bounds(mem_buffer, &start, &end);
	gtk_text_buffer_delete(mem_buffer, &start, &end);

	int i,j;
	GtkTextIter iter;
	gtk_text_buffer_get_iter_at_offset(mem_buffer, &iter, 0);
	int pos = 0;
	char str[mem_buffer_page_size * 80];
	for(i=0; i<mem_buffer_page_size; i++) {
		pos += sprintf(str+pos, "0x%06lX: ", crnt_mem_view_addr + i*16);
		for(j=0; j<16; j++) {
			pos += sprintf(str+pos, "%s", " ");
			pos += sprintf(str+pos, "%02X", machine.memory[crnt_mem_view_addr+i*16+j]);
			if(j == 7)
				pos += sprintf(str+pos, " ");
		}
		pos += sprintf(str+pos, "  ");
		for(j=0; j<16; j++) {
			char c  = machine.memory[crnt_mem_view_addr+i*16+j];
			pos += sprintf(str+pos, "%c", isprint(c) ? c : '.');
		}
		pos += sprintf(str+pos, "\n");
	}
	gtk_text_buffer_insert(mem_buffer, &iter, str, -1);
}


/*static void update_handler(GtkWidget *widget, gpointer data) {
	update_register_data();
	update_mem();
}*/

static void change_PC_value(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	Machine_setReg(&machine, REG_PC, strtol(str, NULL, 0));
	update_register_data();
}
static void change_SW_value(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	Machine_setReg(&machine, REG_SW, strtol(str, NULL, 0));
	update_register_data();
}
static void change_A_value(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	Machine_setReg(&machine, REG_A, strtol(str, NULL, 0));
	update_register_data();
}
static void change_X_value(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	Machine_setReg(&machine, REG_X, strtol(str, NULL, 0));
	update_register_data();
}
static void change_L_value(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	Machine_setReg(&machine, REG_L, strtol(str, NULL, 0));
	update_register_data();
}
static void change_B_value(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	Machine_setReg(&machine, REG_B, strtol(str, NULL, 0));
	update_register_data();
}
static void change_S_value(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	Machine_setReg(&machine, REG_S, strtol(str, NULL, 0));
	update_register_data();
}
static void change_T_value(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	Machine_setReg(&machine, REG_T, strtol(str, NULL, 0));
	update_register_data();
}
static void change_F_value(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	machine.F = strtod(str, NULL);
	update_register_data();
}

static void change_frequency(GtkWidget *widget, gpointer data) {
	if(!running) {
		char *str = (char *) data;
		int freq = strtol(str, NULL, 10);
		if(freq > 0)
			machine.freq = strtol(str, NULL, 10);
	}
	update_frequency_data();
}

static void load_handler(GtkWidget *widget, gpointer data) {
	if(running) return;

	GtkWidget *dialog = gtk_file_chooser_dialog_new("Load OBJ", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					                                  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		FILE *f = fopen(filename, "r");
		Machine_loadObj(&machine, f);
		fclose(f);
		g_free(filename);
	}
	gtk_widget_destroy (dialog);

	update_register_data();
	update_mem();
}

static void load_asm_handler(GtkWidget *widget, gpointer data) {
	if(running) return;

	GtkWidget *dialog = gtk_file_chooser_dialog_new("Load OBJ", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					                                  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		FILE *f = fopen(filename, "r");

		struct Code code;
		Code_init(&code);
		Code_parse(&code, f);

		Code_generate_mem(&code, machine.memory, MAX_ADDRESS, &machine);

		machine.PC = code.end;
		Code_delete(&code);

		fclose(f);
		g_free(filename);
	}
	gtk_widget_destroy (dialog);

	update_register_data();
	update_mem();
}

static void step_handler(GtkWidget *widget, gpointer data) {
	if(running) return;

	Machine_execute(&machine);
	update_register_data();
	update_mem();
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *run_thread(void *ptr) {
	pthread_mutex_lock(&mutex);
	running = 1;
	pthread_mutex_unlock(&mutex);

	//while(run && Machine_run(&machine) > 0);

	long prevPC = -1;
	int group = machine.freq / 100;
	if(group == 0 ) group = 1;
	int ggroup = 10;
	int clocks_for_group = group * (CLOCKS_PER_SEC / machine.freq);
	int count = 0;
	clock_t start = clock();
	clock_t sstart = start;
	//clock_t ssstart = start;
	clock_t sleep_time = 0;
	while(run && machine.PC != prevPC) {
		prevPC = machine.PC;

		Machine_execute(&machine);
		count++;

		if(count % group == 0) {
			sleep_time += (CLOCKS_PER_SEC / 1000000) * (clocks_for_group - (clock() - start));
			if(sleep_time > 1000) {
				usleep(sleep_time);
				sleep_time = 0;
			}

			if(count % (ggroup*group) == 0) {
				clock_t should_be = ggroup * clocks_for_group;
				clock_t diff = clock() - sstart;
				start = sstart = clock();
				sleep_time = (should_be - diff) * (CLOCKS_PER_SEC / 1000000); 
			}
			else
				start = clock();
		}
	}
	//double dddiff = (((double)clock() - ssstart) / (double)CLOCKS_PER_SEC);
	//printf("c: %lf\n", count/dddiff);

	run = 0;

	pthread_mutex_lock(&mutex);
	running = 0;
	pthread_mutex_unlock(&mutex);

	return NULL;
}
gboolean update(gpointer data) {
	pthread_mutex_lock(&mutex);

	update_register_data();
	update_mem();

	gboolean ret =  (run || running ? TRUE : FALSE);

	pthread_mutex_unlock(&mutex);

	return ret;
}
static void run_handler(GtkWidget *widget, gpointer data) {
	if(running) return;
	pthread_t thread;

	run = 1;
	pthread_create(&thread, NULL, run_thread, NULL);
	g_timeout_add(20, update, NULL);
}
static void stop_handler(GtkWidget *widget, gpointer data) {
	run = 0;
}
static void clear_reg_handler(GtkWidget *widget, gpointer data) {
	Machine_clearReg(&machine);
	update_register_data();
}

static void addr_entry_handler(GtkWidget *widget, gpointer data) {
	char *addr = (char *)data;
	crnt_mem_view_addr = (strtol(addr, NULL, 0) / 16) * 16;
	update_mem();
}

GtkEntryBuffer *bchar;
GtkEntryBuffer *bword;
GtkEntryBuffer *bfloat;
static void selection_handler(GtkWidget *widget, gpointer data) {
	GtkTextIter start, end;
	gtk_text_buffer_get_selection_bounds(mem_buffer, &start, &end);
	int line = gtk_text_iter_get_line(&start);
	int column = gtk_text_iter_get_line_offset(&start);
	column = column - 11;
	if(column < 50) {
		if(column > 25)
			column -= 1;
		column /= 3;
	}
	else {
		column -= 50;
	}
	if(column > 15) column = 15;
	if(column < 0) column = 0;
	crnt_mem_sele_addr = crnt_mem_view_addr + line * 16 + column;

	char str[20];

	unsigned char byte = machine.memory[crnt_mem_sele_addr];
	sprintf(str, "0x%02X", byte);
	gtk_entry_buffer_set_text(bchar, str, -1);

	long word = byte;
	if(crnt_mem_sele_addr+2 <= MAX_ADDRESS)
		word = machine.memory[crnt_mem_sele_addr] << 16;
		word |= machine.memory[crnt_mem_sele_addr+1] << 8;
		word |= machine.memory[crnt_mem_sele_addr+2];
	sprintf(str, "0x%06lX", word);
	gtk_entry_buffer_set_text(bword, str, -1);
}
static void edit_char_handler(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	Machine_setByte(&machine, crnt_mem_sele_addr, strtol(str, NULL, 0));
	update_mem();
}
static void edit_word_handler(GtkWidget *widget, gpointer data) {
	char *str = (char *) data;
	Machine_setWord(&machine, crnt_mem_sele_addr, strtol(str, NULL, 0));
	update_mem();
}

static void clear_mem_handler(GtkWidget *widget, gpointer data) {
	Machine_clearMem(&machine);
	update_mem();
}

static GtkWidget *make_machine_box() {
	GtkWidget *machine_box = gtk_vbox_new(FALSE, 0);

	GtkWidget *label = gtk_label_new("Machine");
	
	gtk_box_pack_start(GTK_BOX(machine_box), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	register_data.PC = gtk_entry_buffer_new("", 0);
	register_data.SW = gtk_entry_buffer_new("", 0);
	register_data.A = gtk_entry_buffer_new("", 0);
	register_data.X = gtk_entry_buffer_new("", 0);
	register_data.L = gtk_entry_buffer_new("", 0);
	register_data.B = gtk_entry_buffer_new("", 0);
	register_data.S = gtk_entry_buffer_new("", 0);
	register_data.T = gtk_entry_buffer_new("", 0);
	register_data.F = gtk_entry_buffer_new("", 0);

	update_register_data(machine);

	GtkWidget *lPC = gtk_label_new("PC: ");
	GtkWidget *lSW = gtk_label_new("SW: ");
	GtkWidget *lA = gtk_label_new("A: ");
	GtkWidget *lX = gtk_label_new("X: ");
	GtkWidget *lL = gtk_label_new("L: ");
	GtkWidget *lB = gtk_label_new("B: ");
	GtkWidget *lS = gtk_label_new("S: ");
	GtkWidget *lT = gtk_label_new("T: ");
	GtkWidget *lF = gtk_label_new("F: ");

	GtkWidget *tPC = gtk_entry_new_with_buffer(register_data.PC);
	GtkWidget *tSW = gtk_entry_new_with_buffer(register_data.SW);
	GtkWidget *tA = gtk_entry_new_with_buffer(register_data.A);
	GtkWidget *tX = gtk_entry_new_with_buffer(register_data.X);
	GtkWidget *tL = gtk_entry_new_with_buffer(register_data.L);
	GtkWidget *tB = gtk_entry_new_with_buffer(register_data.B);
	GtkWidget *tS = gtk_entry_new_with_buffer(register_data.S);
	GtkWidget *tT = gtk_entry_new_with_buffer(register_data.T);
	GtkWidget *tF = gtk_entry_new_with_buffer(register_data.F);

	frequency_data = gtk_entry_buffer_new("", 0);
	update_frequency_data();

	GtkWidget *l_freq = gtk_label_new("Hz: ");
	GtkWidget *t_freq = gtk_entry_new_with_buffer(frequency_data);

	GtkWidget *box;
	box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), lPC, FALSE, FALSE, 0);
		gtk_widget_show(lPC);
		gtk_box_pack_end(GTK_BOX(box), tPC, FALSE, FALSE, 0);
		gtk_widget_show(tPC);
	gtk_box_pack_start(GTK_BOX(machine_box), box, FALSE, FALSE, 0);
	gtk_widget_show(box);
	box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), lSW, FALSE, FALSE, 0);
		gtk_widget_show(lSW);
		gtk_box_pack_end(GTK_BOX(box), tSW, FALSE, FALSE, 0);
		gtk_widget_show(tSW);
	gtk_box_pack_start(GTK_BOX(machine_box), box, FALSE, FALSE, 0);
	gtk_widget_show(box);
	box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), lA, FALSE, FALSE, 0);
		gtk_widget_show(lA);
		gtk_box_pack_end(GTK_BOX(box), tA, FALSE, FALSE, 0);
		gtk_widget_show(tA);
	gtk_box_pack_start(GTK_BOX(machine_box), box, FALSE, FALSE, 0);
	gtk_widget_show(box);
	box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), lX, FALSE, FALSE, 0);
		gtk_widget_show(lX);
		gtk_box_pack_end(GTK_BOX(box), tX, FALSE, FALSE, 0);
		gtk_widget_show(tX);
	gtk_box_pack_start(GTK_BOX(machine_box), box, FALSE, FALSE, 0);
	gtk_widget_show(box);
	box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), lL, FALSE, FALSE, 0);
		gtk_widget_show(lL);
		gtk_box_pack_end(GTK_BOX(box), tL, FALSE, FALSE, 0);
		gtk_widget_show(tL);
	gtk_box_pack_start(GTK_BOX(machine_box), box, FALSE, FALSE, 0);
	gtk_widget_show(box);
	box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), lB, FALSE, FALSE, 0);
		gtk_widget_show(lB);
		gtk_box_pack_end(GTK_BOX(box), tB, FALSE, FALSE, 0);
		gtk_widget_show(tB);
	gtk_box_pack_start(GTK_BOX(machine_box), box, FALSE, FALSE, 0);
	gtk_widget_show(box);
	box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), lS, FALSE, FALSE, 0);
		gtk_widget_show(lS);
		gtk_box_pack_end(GTK_BOX(box), tS, FALSE, FALSE, 0);
		gtk_widget_show(tS);
	gtk_box_pack_start(GTK_BOX(machine_box), box, FALSE, FALSE, 0);
	gtk_widget_show(box);
	box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), lT, FALSE, FALSE, 0);
		gtk_widget_show(lT);
		gtk_box_pack_end(GTK_BOX(box), tT, FALSE, FALSE, 0);
		gtk_widget_show(tT);
	gtk_box_pack_start(GTK_BOX(machine_box), box, FALSE, FALSE, 0);
	gtk_widget_show(box);
	box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), lF, FALSE, FALSE, 0);
		gtk_widget_show(lF);
		gtk_box_pack_end(GTK_BOX(box), tF, FALSE, FALSE, 0);
		gtk_widget_show(tF);
	gtk_box_pack_start(GTK_BOX(machine_box), box, FALSE, FALSE, 0);
	gtk_widget_show(box);
	box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), l_freq, FALSE, FALSE, 0);
		gtk_widget_show(l_freq);
		gtk_box_pack_end(GTK_BOX(box), t_freq, FALSE, FALSE, 0);
		gtk_widget_show(t_freq);
	gtk_box_pack_start(GTK_BOX(machine_box), box, FALSE, FALSE, 0);
	gtk_widget_show(box);

	g_signal_connect(tPC, "activate", G_CALLBACK(change_PC_value), (gpointer)gtk_entry_get_text(GTK_ENTRY(tPC)));
	g_signal_connect(tSW, "activate", G_CALLBACK(change_SW_value), (gpointer)gtk_entry_get_text(GTK_ENTRY(tSW)));
	g_signal_connect(tA, "activate", G_CALLBACK(change_A_value), (gpointer)gtk_entry_get_text(GTK_ENTRY(tA)));
	g_signal_connect(tX, "activate", G_CALLBACK(change_X_value), (gpointer)gtk_entry_get_text(GTK_ENTRY(tX)));
	g_signal_connect(tL, "activate", G_CALLBACK(change_L_value), (gpointer)gtk_entry_get_text(GTK_ENTRY(tL)));
	g_signal_connect(tB, "activate", G_CALLBACK(change_B_value), (gpointer)gtk_entry_get_text(GTK_ENTRY(tB)));
	g_signal_connect(tS, "activate", G_CALLBACK(change_S_value), (gpointer)gtk_entry_get_text(GTK_ENTRY(tS)));
	g_signal_connect(tT, "activate", G_CALLBACK(change_T_value), (gpointer)gtk_entry_get_text(GTK_ENTRY(tT)));
	g_signal_connect(tF, "activate", G_CALLBACK(change_F_value), (gpointer)gtk_entry_get_text(GTK_ENTRY(tF)));

	g_signal_connect(t_freq, "activate", G_CALLBACK(change_frequency), (gpointer)gtk_entry_get_text(GTK_ENTRY(t_freq)));

	return machine_box;
}

static GtkWidget *make_menu() {
	GtkWidget *menu = gtk_vbox_new(FALSE, 0);

	//GtkWidget *update = gtk_button_new_with_label("update");
	GtkWidget *load = gtk_button_new_with_label("load");
	GtkWidget *load_asm = gtk_button_new_with_label("load asm");
	GtkWidget *step = gtk_button_new_with_label("step");
	GtkWidget *run = gtk_button_new_with_label("run");
	GtkWidget *stop = gtk_button_new_with_label("stop");
	GtkWidget *clear = gtk_button_new_with_label("clear");

	//g_signal_connect(update, "clicked", G_CALLBACK(update_handler), NULL);
	g_signal_connect(load, "clicked", G_CALLBACK(load_handler), NULL);
	g_signal_connect(load_asm, "clicked", G_CALLBACK(load_asm_handler), NULL);
	g_signal_connect(step, "clicked", G_CALLBACK(step_handler), NULL);
	g_signal_connect(run, "clicked", G_CALLBACK(run_handler), NULL);
	g_signal_connect(stop, "clicked", G_CALLBACK(stop_handler), NULL);
	g_signal_connect(clear, "clicked", G_CALLBACK(clear_reg_handler), NULL);

	//gtk_box_pack_start(GTK_BOX(menu), update, FALSE, FALSE, 0);
	//gtk_widget_show(update);
	gtk_box_pack_start(GTK_BOX(menu), load, FALSE, FALSE, 0);
	gtk_widget_show(load);
	gtk_box_pack_start(GTK_BOX(menu), load_asm, FALSE, FALSE, 0);
	gtk_widget_show(load_asm);
	gtk_box_pack_start(GTK_BOX(menu), step, FALSE, FALSE, 0);
	gtk_widget_show(step);
	gtk_box_pack_start(GTK_BOX(menu), run, FALSE, FALSE, 0);
	gtk_widget_show(run);
	gtk_box_pack_start(GTK_BOX(menu), stop, FALSE, FALSE, 0);
	gtk_widget_show(stop);
	gtk_box_pack_start(GTK_BOX(menu), clear, FALSE, FALSE, 0);
	gtk_widget_show(clear);

	return menu;
}

static GtkWidget *make_left_box() {
	GtkWidget *left = gtk_vbox_new(FALSE, 5);

	GtkWidget *machine_box = make_machine_box();
	GtkWidget *menu = make_menu();

	gtk_box_pack_start(GTK_BOX(left), machine_box, FALSE, FALSE, 0);
	gtk_widget_show(machine_box);

	gtk_box_pack_start(GTK_BOX(left), menu, FALSE, FALSE, 0);
	gtk_widget_show(menu);

	return left;
}

void adjustment_handler(GtkAdjustment *adjustment, gpointer addr_buffer) {
	crnt_mem_view_addr = (long) adjustment->value * 16;
	char str[10];
	sprintf(str, "0x%06lX", crnt_mem_view_addr);
	gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER((GtkWidget *)addr_buffer), str, -1);
	update_mem();
}
static GtkWidget *make_mem_box() {
	GtkWidget *mem_box = gtk_vbox_new(FALSE, 5);

	GtkWidget *mmem_box = gtk_hbox_new(FALSE, 2);

	GtkEntryBuffer *addr_buffer = gtk_entry_buffer_new("0x000000", -1);
	GtkWidget *addr_entry = gtk_entry_new_with_buffer(addr_buffer);

	g_signal_connect(addr_entry, "activate", G_CALLBACK(addr_entry_handler), (gpointer)gtk_entry_get_text(GTK_ENTRY(addr_entry)));

	gtk_box_pack_start(GTK_BOX(mem_box), addr_entry, FALSE, FALSE, 0);
	gtk_widget_show(addr_entry);

	GtkWidget *view = gtk_text_view_new();

	PangoFontDescription *font_desc = pango_font_description_from_string("Monospace");
	gtk_widget_modify_font(view, font_desc);
	pango_font_description_free(font_desc);

	gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);

	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 5);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(view), 5);

	mem_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

	update_mem();

	g_signal_connect(mem_buffer, "mark-set", G_CALLBACK(selection_handler), NULL);

	GtkObject *adjustment = gtk_adjustment_new(0, 0, MAX_ADDRESS/16 + 1, 1, mem_buffer_page_size, mem_buffer_page_size);
	GtkWidget *scrollbar = gtk_vscrollbar_new(GTK_ADJUSTMENT(adjustment));

	g_signal_connect(adjustment, "value_changed", G_CALLBACK(adjustment_handler), addr_buffer);

	gtk_box_pack_start(GTK_BOX(mmem_box), view, FALSE, FALSE, 0);
	gtk_widget_show(view);

	gtk_box_pack_start(GTK_BOX(mmem_box), scrollbar, FALSE, FALSE, 0);
	gtk_widget_show(scrollbar);

	GtkWidget *edit_box = gtk_hbox_new(FALSE, 3);

	GtkWidget *lchar = gtk_label_new("byte: ");
	gtk_box_pack_start(GTK_BOX(edit_box), lchar, FALSE, FALSE, 0);
	gtk_widget_show(lchar);
	bchar = gtk_entry_buffer_new("", 0);
	GtkWidget *echar = gtk_entry_new_with_buffer(bchar);
	gtk_box_pack_start(GTK_BOX(edit_box), echar, FALSE, FALSE, 0);
	gtk_widget_show(echar);

	g_signal_connect(echar, "activate", G_CALLBACK(edit_char_handler), (gpointer)gtk_entry_get_text(GTK_ENTRY(echar)));

	GtkWidget *lword = gtk_label_new("word: ");
	gtk_box_pack_start(GTK_BOX(edit_box), lword, FALSE, FALSE, 0);
	gtk_widget_show(lword);
	bword = gtk_entry_buffer_new("", 0);
	GtkWidget *eword = gtk_entry_new_with_buffer(bword);
	gtk_box_pack_start(GTK_BOX(edit_box), eword, FALSE, FALSE, 0);
	gtk_widget_show(eword);

	g_signal_connect(eword, "activate", G_CALLBACK(edit_word_handler), (gpointer)gtk_entry_get_text(GTK_ENTRY(eword)));

	/*GtkWidget *lfloat = gtk_label_new("float: ");
	gtk_box_pack_start(GTK_BOX(edit_box), lfloat, FALSE, FALSE, 0);
	gtk_widget_show(lfloat);
	bfloat = gtk_entry_buffer_new("", 0);
	GtkWidget *efloat = gtk_entry_new_with_buffer(bfloat);
	gtk_box_pack_start(GTK_BOX(edit_box), efloat, FALSE, FALSE, 0);
	gtk_widget_show(efloat);
	*/

	GtkWidget *clear = gtk_button_new_with_label("clear");
	g_signal_connect(clear, "clicked", G_CALLBACK(clear_mem_handler), NULL);
	gtk_box_pack_start(GTK_BOX(edit_box), clear, FALSE, FALSE, 0);
	gtk_widget_show(clear);

	gtk_box_pack_start(GTK_BOX(mem_box), mmem_box, FALSE, FALSE, 0);
	gtk_widget_show(mmem_box);

	gtk_box_pack_start(GTK_BOX(mem_box), edit_box, FALSE, FALSE, 0);
	gtk_widget_show(edit_box);

	return mem_box;
}

static GtkWidget *make_box() {
	GtkWidget *box = gtk_hbox_new(FALSE, 10);

	GtkWidget *left_box = make_left_box();
	GtkWidget *mem_box = make_mem_box();

	gtk_box_pack_start(GTK_BOX(box), left_box, FALSE, FALSE, 0);
	gtk_widget_show(left_box);

	gtk_box_pack_start(GTK_BOX(box), mem_box, FALSE, FALSE, 0);
	gtk_widget_show(mem_box);

	return box;
}

static GtkWidget *make_window() {
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
	gtk_window_set_title(GTK_WINDOW(window), "gsim");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	return window;
}

int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv);

	GtkWidget *window = make_window();

	Machine_init(&machine);

	GtkWidget *box = make_box();
	gtk_container_add(GTK_CONTAINER(window), box);
	gtk_widget_show(box);

	gtk_widget_show(window);
	gtk_main();

	Machine_delete(&machine);

	return 0;
}
