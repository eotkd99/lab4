#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <gtk/gtk.h>

static float num1 = 0;
static char lastChar = (char)0;
static char prevCmd = (char)0;
#define BUF_SIZE 88
typedef struct {

	char* szLabel;   
	int       row;      
	int       col;        
	GtkWidget* widget;    

} typCalculatorButton;
typCalculatorButton buttonList[] = {
	{"C",   1, 0, NULL},     
	{"CE",  1, 1, NULL},     
	{"/",   1, 3, NULL},     

	{"7",   2, 0, NULL},      
	{"8",   2, 1, NULL},     
	{"9",   2, 2, NULL},      
	{"*",   2, 3, NULL},     
	{"%",   2, 4, NULL},     

	{"4",   3, 0, NULL},      
	{"5",   3, 1, NULL},      
	{"6",   3, 2, NULL},      
	{"-",   3, 3, NULL},      
	{"1/x", 3, 4, NULL},      

	{"1",   4, 0, NULL},      
	{"2",   4, 1, NULL},     
	{"3",   4, 2, NULL},      
	{"+",   4, 3, NULL},     
	{"sqrt",4, 4, NULL},     

	{"+/-", 5, 0, NULL},      
	{"0",   5, 1, NULL},    
	{".",   5, 2, NULL},     
	{"=",   5, 3, NULL},    
	{"x^2", 5, 4, NULL},      
};
int nButtons = sizeof(buttonList) /sizeof(typCalculatorButton);
GtkWidget* label;
int CloseAppWindow(GtkWidget* widget, gpointer data)
{
	gtk_main_quit();

	return (FALSE);
}
void TrimTrailingZeros(char* szDigits)
{
	int nIndex;
	int bDecimal = FALSE;
	int nPos = -1;
	for (nIndex = 0; nIndex < strlen(szDigits); nIndex++) {
		if (szDigits[nIndex] == '.') {
			bDecimal = TRUE;
		}
		if (bDecimal) {
			if (szDigits[nIndex] == '0') {
				if (nPos < 0) {
					nPos = nIndex;
				}
			}
			else {
				nPos = -1;
			}
		}
	}
	if (nPos > 0) {
		szDigits[nPos] = (char)0;
	}
}
void TrimLeadingZeros(char* szDigits)
{
	int nPos;
	if (szDigits == NULL) return;
	for (nPos = 0; (szDigits[nPos] && szDigits[nPos] == '0'); nPos++) {
		if (isdigit(szDigits[nPos + 1])) {
			szDigits[nPos] = ' ';
		}
	}
}
int Command(char ch)
{
	switch (ch) {
	case '+':
	case '-':
	case '/':
	case '*':
	case '=':
		return (TRUE);
	}
	return (FALSE);
}
int FloatingPointChar(char ch)
{
	return (isdigit(ch) || ch == '.');
}
void key_press(GtkWidget* widget,GdkEventKey* event,gpointer data){
	int nIndex;
	for (nIndex = 0; nIndex < nButtons; nIndex++) {
		if (event->keyval == buttonList[nIndex].szLabel[0] &&
			buttonList[nIndex].szLabel[1] == (char)0) {
			gtk_widget_grab_focus(buttonList[nIndex].widget);
			gtk_button_clicked(GTK_BUTTON(buttonList[nIndex].widget));
			return;
		}
	}
}
void HandleDigit(char* str, char ch)
{
	char* labelText;
	char buffer[BUF_SIZE];
	int  len;
	if (Command(lastChar)) {
		gtk_label_set(GTK_LABEL(label), "");
		if (lastChar == '=') {
			lastChar = (char)0;
			prevCmd = (char)0;
		}
	}
	gtk_label_get(GTK_LABEL(label), &labelText);
	strcpy(buffer, labelText);
	len = strlen(buffer);
	buffer[len] = (gchar)ch;
	buffer[len + 1] = (gchar)0;
	TrimLeadingZeros(buffer);
	gtk_label_set(GTK_LABEL(label), (char*)buffer);
}
void MaybeUnaryOperation(char* str)
{
	char* labelText;
	char buffer[BUF_SIZE];
	float num2;
	gtk_label_get(GTK_LABEL(label), &labelText);
	num2 = atof(labelText);
	if (strcmp(str, "%") == 0) {
		num2 = num2 / 100;
	}
	else if (strcmp(str, "1/x") == 0) {
		if (num2 == 0) {
			return;
		}
		num2 = 1 / num2;
	}
	else if (strcmp(str, "sqrt") == 0) {
		num2 = sqrt((double)num2);
	}
	else if (strcmp(str, "x^2") == 0) {
		num2 = num2 * num2;
	}
	sprintf(buffer, "%f", (float)num2);
	TrimTrailingZeros(buffer);
	TrimLeadingZeros(buffer);
	gtk_label_set(GTK_LABEL(label), buffer);
}


void HandleBinaryOperation()
{
	char buffer[BUF_SIZE];
	char* labelText;
	float num2;
	gtk_label_get(GTK_LABEL(label), &labelText);
	num2 = atof(labelText);
	switch (prevCmd) {
	case '+':
		num1 = num1 + num2;
		break;

	case '-':
		num1 = num1 - num2;
		break;

	case '*':
		num1 = num1 * num2;
		break;

	case '/':
		num1 = num1 / num2;
		break;

	case '=':
		num1 = num2;
		break;

	default:
		num1 = num2;
		break;
	}
	sprintf(buffer, "%f", (float)num1);
	TrimTrailingZeros(buffer);
	TrimLeadingZeros(buffer);
	gtk_label_set(GTK_LABEL(label), buffer);
}
void button_clicked(GtkWidget* widget, gpointer data)
{
	char ch = *((char*)data);
	char* str;
	str = (char*)data;
	if (FloatingPointChar(ch) && strlen(str) == 1) {
		HandleDigit(str, ch);
	}
	else {
		if (strcmp(str, "CE") == 0) {
			gtk_label_set(GTK_LABEL(label), "0");
			return;
		}
		else if (strcmp(str, "C") == 0) {
			prevCmd = (char)0;
			lastChar = (char)0;
			gtk_label_set(GTK_LABEL(label), "0");
			return;

		}
		else {
			MaybeUnaryOperation(str);
		}
		HandleBinaryOperation();
		prevCmd = ch;
	}
	lastChar = ch;
}
GtkWidget* CreateButton(GtkWidget* table, char* szLabel, int row, int column){
	GtkWidget* button;
	button = gtk_button_new_with_label(szLabel);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",GTK_SIGNAL_FUNC(button_clicked), szLabel);
	gtk_table_attach(GTK_TABLE(table), button,	column, column + 1,	row, row + 1,GTK_FILL | GTK_EXPAND,	GTK_FILL | GTK_EXPAND,5, 5);
	gtk_widget_show(button);
	return (button);
}
void CreateCalculatorButtons(GtkWidget* table){
	int nIndex;
	for (nIndex = 0; nIndex < nButtons; nIndex++) {
		buttonList[nIndex].widget =CreateButton(table,buttonList[nIndex].szLabel,buttonList[nIndex].row,buttonList[nIndex].col);
	}
}
int main(int argc, char* argv[])
{
	GtkWidget* window;
	GtkWidget* table;
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Calculator");
	gtk_widget_set_usize(window, 200, 200);
	gtk_signal_connect(GTK_OBJECT(window), "key_press_event", GTK_SIGNAL_FUNC(key_press), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(CloseAppWindow), NULL);
	table = gtk_table_new(5, 5, TRUE);
	CreateCalculatorButtons(table);
	label = gtk_label_new("0");
	gtk_misc_set_alignment(GTK_MISC(label), 1, .5);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 4, 0, 1);
	gtk_widget_show(label);
	gtk_container_add(GTK_CONTAINER(window), table);
	gtk_widget_show(table);
	gtk_widget_show(window);
	gtk_main();
	return (0);
}
