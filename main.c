#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "btree.h"
#include <time.h>

BTA *data;
GtkWidget *textView,*mainwindow;
GtkTextBuffer *buffer;
time_t t_begin, t_end;
// Soundex

int SoundEx(char *SoundEx,
              char *WordString,
              int   LengthOption,
              int   CensusOption)
  {
      int  InSz = 31;
      char WordStr[32];     /* one bigger than InSz */
      int  SoundExLen, WSLen, i;
      char FirstLetter, *p, *p2;

      SoundExLen = WSLen = 0;
      SoundEx[0] = 0;

      if(CensusOption) {
        LengthOption = 4;
      }

      if(LengthOption) {
          SoundExLen = LengthOption;
      }
      if(SoundExLen > 10) {
          SoundExLen = 10;
      }
      if(SoundExLen < 4) {
          SoundExLen = 4;
      }

      if(!WordString) {
          return(0);
      }

      /* Copy WordString to WordStr
       * without using funcs from other
       * libraries.
      */
      for(p = WordString,p2 = WordStr,i = 0;(*p);p++,p2++,i++) {
        if(i >= InSz) break;
        (*p2) = (*p);
      }
      (*p2) = 0;

      /* Convert WordStr to
       * upper-case, without using funcs
       * from other libraries
      */
      for(p = WordStr;(*p);p++) {
        if( (*p) >= 'a' && (*p) <= 'z' ) {
            (*p) -= 0x20;
        }
      }

      /* convert all non-alpha
       * chars to spaces
      */
      for(p = WordStr;(*p);p++) {
        if( (*p) < 'A' || (*p) > 'Z' ) {
            (*p) = ' ';
        }
      }

      /* Remove leading spaces
      */
      for(i = 0, p = p2 = WordStr;(*p);p++) {
          if(!i) {
              if( (*p) != ' ' ) {
                  (*p2) = (*p);
                  p2++;
                  i++;
              }
          }
          else {
              (*p2) = (*p);
              p2++;
          }
      }
      (*p2) = 0;

      /* Get length of WordStr
      */
      for(i = 0,p = WordStr;(*p);p++) i++;


      /* Remove trailing spaces
      */
      for(;i;i--) {
          if(WordStr[i] == ' ') {
            WordStr[i] = 0;
          }
          else {
              break;
          }
      }

      /* Get length of WordStr
      */
      for(WSLen = 0,p = WordStr;(*p);p++) WSLen++;

      if(!WSLen) {
        return(0);
      }

      /* Perform our own multi-letter
       * improvements
       *
       * underscore placeholders (_) will be
       * removed below.
      */
      if(!CensusOption) {
          if(WordStr[0] == 'P' && WordStr[1] == 'S') {
              WordStr[0] = '_';
          }

          if(WordStr[0] == 'P' && WordStr[1] == 'F') {
              WordStr[0] = '_';
          }

          /* v1.0e gh is G-sound at start of word
          */
          if(WordStr[0] == 'G' && WordStr[1] == 'H') {
              WordStr[1] = '_';
          }

          for(i = 0;i < WSLen;i++) {
              if(WordStr[i] == 'D' && WordStr[i+1] == 'G') {
                  WordStr[i] = '_';
                  i++;
                  continue;
              }
              if( WordStr[i] == 'G' && WordStr[i+1] == 'H') {
                  WordStr[i] = '_';
                  i++;
                  continue;
              }
              if(WordStr[i] == 'K' && WordStr[i+1] == 'N') {
                  WordStr[i] = '_';
                  i++;
                  continue;
              }
              if(WordStr[i] == 'G' && WordStr[i+1] == 'N') {
                  WordStr[i] = '_';
                  i++;
                  continue;
              }
              if(WordStr[i] == 'M' && WordStr[i+1] == 'B') {
                  WordStr[i+1] = '_';
                  i++;
                  continue;
              }

              if(WordStr[i] == 'P' && WordStr[i+1] == 'H') {
                  WordStr[i] = 'F';
                  WordStr[i+1] = '_';
                  i++;
                  continue;
              }
              if(WordStr[i]  ==  'T'  &&
                 WordStr[i+1] == 'C' &&
                 WordStr[i+2] == 'H'
                ) {

                  WordStr[i] = '_';
                  i++; i++;
                  continue;
              }
              if(WordStr[i] == 'M' && WordStr[i+1] == 'P'
                 && (WordStr[i+2] == 'S' ||
                     WordStr[i+2] == 'T' ||
                     WordStr[i+2] == 'Z')
                ) {
                  WordStr[i+1] = '_';
                  i++;
              }
          }
      } /* end if(!CensusOption) */



      /* squeeze out underscore characters
       * added as a byproduct of above process
       * (only needed in c styled replace)
      */
      for(p = p2 = WordStr;(*p);p++) {
        (*p2) = (*p);
        if( (*p2) != '_' ) {
            p2++;
        }
      }
      (*p2) = 0;

      /* This must be done AFTER our
       * multi-letter replacements
       * since they could change
       * the first letter
      */
      FirstLetter = WordStr[0];


      /* In case we're in CensusOption
       * 1 and the word starts with
       * an 'H' or 'W'
       *  (v1.0c djr: add test for H or W)
      */
      if(FirstLetter == 'H' || FirstLetter == 'W') {
          WordStr[0] = '-';
      }

      /* In properly done census
       * SoundEx, the H and W will
       * be squeazed out before
       * performing the test
       * for adjacent digits
       * (this differs from how
       * 'real' vowels are handled)
      */
      if(CensusOption == 1) {
          for(p = &(WordStr[1]);(*p);p++) {
             if((*p) == 'H' || (*p) == 'W') {
                 (*p) = '.';
             }
          }
      }

      /* Perform classic SoundEx
       * replacements.
      */
      for(p = WordStr;(*p);p++) {
          if( (*p) == 'A'   ||
              (*p) == 'E'   ||
              (*p) == 'I'   ||
              (*p) == 'O'   ||
              (*p) == 'U'   ||
              (*p) == 'Y'   ||
              (*p) == 'H'   ||
              (*p) == 'W'
            ){
              (*p) = '0';   /* zero */
          }
          if( (*p) == 'B'   ||
              (*p) == 'P'   ||
              (*p) == 'F'   ||
              (*p) == 'V'
            ){
              (*p) = '1';
          }
          if( (*p) == 'C'   ||
              (*p) == 'S'   ||
              (*p) == 'G'   ||
              (*p) == 'J'   ||
              (*p) == 'K'   ||
              (*p) == 'Q'   ||
              (*p) == 'X'   ||
              (*p) == 'Z'
            ){
              (*p) = '2';
          }
          if( (*p) == 'D'   ||
              (*p) == 'T'
            ){
              (*p) = '3';
          }
          if( (*p) == 'L' ) {
              (*p) = '4';
          }

          if( (*p) == 'M'   ||
              (*p) == 'N'
            ){
              (*p) = '5';
          }
          if( (*p) == 'R' ) {
              (*p) = '6';
          }
      }
      /* soundex replacement loop done  */

      /* In properly done census
       * SoundEx, the H and W will
       * be squezed out before
       * performing the test
       * for adjacent digits
       * (this differs from how
       * 'real' vowels are handled)
      */
      if(CensusOption == 1) {
          /* squeeze out dots
          */
          for(p = p2 = &WordStr[1];(*p);p++) {
            (*p2) = (*p);
            if( (*p2) != '.' ) {
                p2++;
            }
          }
          (*p2) = 0;
      }

      /* squeeze out extra equal adjacent digits
       * (don't include first letter)
       * v1.0c djr (now includes first letter)
      */
      for(p = p2 = &(WordStr[0]);(*p);p++) {
        (*p2) = (*p);
        if( (*p2) != p[1] ) {
            p2++;
        }
      }
      (*p2) = 0;

      /* squeeze out spaces and zeros
       * Leave the first letter code
       * to be replaced below.
       * (In case it made a zero)
      */
      for(p = p2 = &WordStr[1];(*p);p++) {
        (*p2) = (*p);
        if( (*p2) != ' ' && (*p2) != '0' ) {
            p2++;
        }
      }
      (*p2) = 0;

      /* Get length of WordStr
      */
      for(WSLen = 0,p = WordStr;(*p);p++) WSLen++;

      /* Right pad with zero characters
      */
      for(i = WSLen;i < SoundExLen;i++ ) {
          WordStr[i] = '0';
      }

      /* Size to taste
      */
      WordStr[SoundExLen] = 0;


      /* Replace first digit with
       * first letter.
      */
      WordStr[0] = FirstLetter;


      /* Copy WordStr to SoundEx
      */
      for(p2 = SoundEx,p = WordStr;(*p);p++,p2++) {
          (*p2) = (*p);
      }
      (*p2) = 0;
      
      return(SoundExLen);
  }

// So sanh 2 tu bang Soundex

int suggest_word (char *source, char *target) {
  char str1[10], str2[10];

  SoundEx(str1, source, 0, 2);
  SoundEx(str2, target, 0, 2);

  return strcmp (str1, str2);
}

//Tim kiem va hien nghia

void *show_meaning(char *meaning){
  FILE *stream;
  GtkTextIter iter;
  char line[2000];
  static int created=0;
  // Ham chuyen 1 string tuong duong thanh 1 file
  stream=fmemopen(meaning,strlen(meaning)+1,"r");
  gtk_text_buffer_set_text(buffer,"",-1);
  gtk_text_buffer_get_start_iter(buffer,&iter);
  //Tag kieu chu
  if (!created){
    gtk_text_buffer_create_tag(buffer,"lmarg15","left_margin",15,NULL);
    gtk_text_buffer_create_tag(buffer,"lmarg30","left_margin",30,NULL);
    gtk_text_buffer_create_tag(buffer,"lmarg35","left_margin",35,NULL);
    gtk_text_buffer_create_tag(buffer,"bold","weight",PANGO_WEIGHT_BOLD,NULL);
    created=1;
  }
  //In nghia
  while (fgets(line,2000,stream)!=NULL){
    switch(line[0]){
    case '@':
      line[0]=' ';
      gtk_text_buffer_insert(buffer,&iter,"\n",-1);
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"bold",NULL);
      gtk_text_buffer_insert(buffer,&iter,"\n",-1);
      break;
    case '*':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg15",NULL);
      break;
    case '!':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg15",NULL);
      break;
    case '-':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg30",NULL);
      break;
    case '=':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg35",NULL);
      break;
    default:
      gtk_text_buffer_insert(buffer,&iter,line,-1);
      break;
    }
  }
}

gboolean display(GtkWidget *entryword,gpointer database){
  GtkWidget *dialog,*window=mainwindow;
  char word[50],mean[10000];
  int value;
  BTint valnext;
  strcpy(word,(char *)gtk_entry_get_text(GTK_ENTRY(entryword)));
  if (strlen(word) == 0)
  {
    strcpy(mean,"Bạn chưa nhập từ vào ô tìm kiếm!");
  }
  else
  {
  	if (bfndky(data,word,(BTint *)&value)==0)
  	{
    	btsel(data,word,mean,10000,&value);
  	}
  	else
  	{
      //Tim va hien thi suggestion word neu co
      strcpy(mean,"Xin lỗi! Từ này chưa có trong dữ liệu! \n\n Bạn có thể thêm từ này vào từ điển!\n");
      int index = 0, count = 0;
      char next_near_word[100];
      bfndky(data,word,&valnext);
      while ((index <= 100) && (count < 1)){
        bnxtky(data, next_near_word, &valnext);
        if (suggest_word (word, next_near_word) == 0) {
          strcat (mean, "Gợi ý :\n");
          strcat (mean, next_near_word);
          strcat (mean, "\n");
          count++;
        } else
        
        index++;
      }
  	}
  }
  show_meaning(mean);
}

int isPrefix(char *s,char *word){
  char *pdest = strstr (word, s);
  int result = (int) (pdest - word + 1);
  return result;
}

gboolean searchword(GtkWidget *entryword,GdkEvent *event,gpointer listword){
  GdkEventKey *key=(GdkEventKey *)event;
  GtkListStore *liststore=(GtkListStore *)listword;
  GtkTreeIter iter;
  int count=0,len;
  int value;
  char text[50],near_word[50], *mean = (char *)malloc (10000);

  // Auto-complete Tab
  t_begin = clock();

  if (key->keyval==GDK_KEY_Tab){
    strcpy(text,(char*)gtk_entry_get_text(GTK_ENTRY (entryword)));
    if (btsel(data,text, mean, 10000,&value)==QNOKEY){
      btseln(data,near_word,mean, 10000, &value);
      if (isPrefix(text,near_word) == 1){
        gtk_entry_set_text(GTK_ENTRY(entryword),near_word);
        gtk_editable_set_position(GTK_EDITABLE(entryword),strlen(near_word));
        }
      else return TRUE;
    }
    display(entryword,NULL);
    
    return TRUE;
  }
  else{

    //count=0;
    // Suggestion search

    strcpy(text,gtk_entry_get_text((GtkEntry*) entryword));

    if (key->keyval!=GDK_KEY_BackSpace){
      len=strlen(text);
      text[len]=key->keyval;
      text[len+1]='\0';
    }
    gtk_list_store_clear(liststore);
    if (btsel(data,text,mean, 10000, &value) == 0) {
      gtk_list_store_append(liststore,&iter);
      gtk_list_store_set(liststore,&iter,0,text,-1);
    }
    btseln(data,near_word,mean, 10000, &value);
    while ((isPrefix(text,near_word) == 1) && (count < 8)){
      gtk_list_store_append(liststore,&iter);
      gtk_list_store_set(liststore,&iter,0,near_word,-1);
      btseln(data,near_word,mean, 10000, &value);
      count++;
    }
  
    t_end = clock();
    g_print ("Thoi gian ham searchword: %lf\n", (double)(t_end - t_begin)/CLOCKS_PER_SEC);
    return FALSE;
  }
}

//dialog

gboolean show_info(GtkWidget *widget,gpointer database){
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_file("info.png",NULL);
  GtkWidget *dialog=gtk_about_dialog_new();
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog),"Từ điển Anh-Việt");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog),"Version 0.0");
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),"(c) Việt Nhật C K60\n\nTrịnh Hữu Vũ\nDương Ngọc Sơn \nĐỗ Thị Hải \nNguyễn Thị Kiều Thương\n");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),"Chương trình được tham khảo code từ trang zetcode.com và project của khóa K58\n");
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog),pixbuf);
  g_object_unref(pixbuf),pixbuf=NULL;
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

gboolean show_guide(GtkWidget *widget,gpointer database){
  GtkWidget *dialog,*window;
  window=mainwindow;
  dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"1. Để tra từ, nhập từ sau đó nhấn Enter hoặc click vào nút Tra từ\n2. Để thêm từ nhấn Thêm từ rồi nhập từ và nghĩa, sau đó ấn nút add.\n3. Để xóa từ, nhập từ và click vào nút Xóa từ\n4. Để đưa về màn hình ban đầu, click vào nút Khôi phục\n");
  gtk_window_set_title(GTK_WINDOW(dialog),"Hướng dẫn sử dụng");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

// Them tu

gboolean add_word(GtkWidget *dialog,gpointer entry){
  GtkWidget *dialog1;
  GtkWidget *word,*mean;
  GtkWidget *add,*cancel;
  GtkWidget *word_entry,*textView;
  GtkTextBuffer *buffer;
  

  dialog=gtk_dialog_new_with_buttons("Thêm từ",mainwindow,GTK_DIALOG_MODAL,GTK_STOCK_ADD,1,GTK_STOCK_CANCEL,2,NULL);
  gtk_window_set_title(GTK_WINDOW(dialog),"Thêm từ");
  gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 300);
  gtk_dialog_set_default_response(GTK_DIALOG(dialog),1);
  word=gtk_label_new("Từ");
  mean=gtk_label_new("Nghĩa");
  word_entry=gtk_entry_new();

  GtkWidget* scrolledWindow=gtk_scrolled_window_new(NULL,NULL);
  gtk_container_set_border_width(GTK_CONTAINER (scrolledWindow),10);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  textView=gtk_text_view_new();
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView),GTK_WRAP_WORD);
  buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
  gtk_container_add(GTK_CONTAINER(scrolledWindow),textView);
  gtk_widget_set_size_request (scrolledWindow, 200, 200);

  GtkWidget *table;
  table=gtk_table_new(5,5,FALSE);
  gtk_table_attach_defaults(GTK_TABLE(table),word,0,1,0,1);
  gtk_table_attach_defaults(GTK_TABLE(table),mean,0,1,1,2);
  gtk_table_attach_defaults(GTK_TABLE(table),word_entry,1,4,0,1);
  gtk_table_attach_defaults(GTK_TABLE(table),scrolledWindow,1,4,1,4);
  gtk_table_set_row_spacings (GTK_TABLE (table),10);
  gtk_table_set_col_spacings (GTK_TABLE (table),3);
  gtk_container_set_border_width(GTK_CONTAINER(textView),1);
  gtk_container_set_border_width(GTK_CONTAINER(scrolledWindow),1);
  gtk_container_set_border_width (GTK_CONTAINER (table),2);
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), table, 0, 0, 0);
  
  gtk_widget_show_all(dialog);
  
  GtkTextIter start,end;
  char *s = (char*)malloc (200), *meaning = (char*)malloc (10000);
  int k;
  BTint value;
  int result=gtk_dialog_run(GTK_DIALOG(dialog));

  //Thoi gian thuc hien ham

  t_begin = clock();

  if (result==1) {
    gtk_text_buffer_get_bounds(buffer,&start,&end);
    strcpy(s, gtk_entry_get_text((GtkEntry*)word_entry));   
    strcpy(meaning, gtk_text_buffer_get_text(buffer, &start, &end, FALSE));
    if (strlen(s)==0){
      dialog1=gtk_message_dialog_new(GTK_WINDOW(dialog),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Bạn chưa nhập từ!");
      gtk_window_set_title(GTK_WINDOW(dialog1),"Lỗi!");
      gtk_dialog_run(GTK_DIALOG(dialog1));
      t_begin = clock();
      t_end = clock();
      g_print ("Thoi gian chuc nang them tu: %lf\n", (double)(t_end - t_begin)/CLOCKS_PER_SEC);
    }
    else if (bfndky(data,s,&value)!=QNOKEY){
      dialog1=gtk_message_dialog_new(GTK_WINDOW(dialog),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Từ đã có trong cơ sở dữ liệu!Bạn muốn cập nhập nghĩa của từ này?");
      gtk_window_set_title(GTK_WINDOW(dialog1),"Trùng từ");
      if (gtk_dialog_run(GTK_DIALOG(dialog1))==GTK_RESPONSE_YES)
      	t_begin = clock();
        btupd(data,s,meaning,strlen(meaning)+1);
        t_end = clock();
  		g_print ("Thoi gian chuc nang sua tu: %lf\n", (double)(t_end - t_begin)/CLOCKS_PER_SEC);
      }
    else{
      dialog1=gtk_message_dialog_new(GTK_WINDOW(dialog),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Bạn muốn thêm từ này vào cơ sở dữ liệu?");
      gtk_window_set_title(GTK_WINDOW(dialog1),"Xác nhận");
      if (gtk_dialog_run(GTK_DIALOG(dialog1))==GTK_RESPONSE_YES) {
      	t_begin = clock();
        btins(data,s ,meaning,strlen(meaning)+1);
        t_end = clock();
  		g_print ("Thoi gian chuc nang them tu: %lf\n", (double)(t_end - t_begin)/CLOCKS_PER_SEC);
      }
    }  
  }
  
  free (s);
  free (meaning);
  gtk_widget_destroy(dialog);
}

// Xoa tu

gboolean delete_word(GtkWidget *widget, gpointer entry)
{
  GtkWidget *dialog,*window=mainwindow;
  char word[50];
  BTint value;


  strcpy(word,(char*)gtk_entry_get_text(GTK_ENTRY(entry)));
  if (strlen(word) == 0)
  {
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Bạn chưa nhập từ!");
    gtk_window_set_title(GTK_WINDOW(dialog),"Lỗi!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    t_begin = clock();
    t_end = clock();
  	g_print ("Thoi gian ham delete_word: %lf\n", (double)(t_end - t_begin)/CLOCKS_PER_SEC);
  }
  else if (bfndky(data,word,&value)==0)
  {
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Bạn muốn xóa từ này khỏi cơ sở dữ liệu?");
    gtk_window_set_title(GTK_WINDOW(dialog),"Xác nhận");
    if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_YES) 
    {
      t_begin = clock();
      btdel(data, word);
      t_end = clock();
      g_print ("Thoi gian ham delete_word: %lf\n", (double)(t_end - t_begin)/CLOCKS_PER_SEC);
      gtk_text_buffer_set_text(buffer,"",-1);
      gtk_entry_set_text(GTK_ENTRY(entry),"");
    }
  }
  else
  {
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Từ không có trong cơ sở dữ liệu!");
    gtk_window_set_title(GTK_WINDOW(dialog),"Lỗi!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    t_begin = clock();
    t_end = clock();
  	g_print ("Thoi gian ham delete_word: %lf\n", (double)(t_end - t_begin)/CLOCKS_PER_SEC);
  }  

  gtk_widget_destroy(dialog);
  return FALSE;
}

gboolean search_word(GtkWidget *button,gpointer database)
{
  display(GTK_WIDGET(database),NULL);
  return TRUE;
}

gboolean delete_event(GtkWidget *widget,GdkEvent *event,gpointer database)
{
    gtk_main_quit();
    return FALSE;
}

gboolean clearAll(GtkWidget *widget,gpointer database)
{
    gtk_text_buffer_set_text(buffer,"", -1);
    gtk_entry_set_text(GTK_ENTRY(database),"");
    return FALSE;
}

//button

void AnhViet(){
  btcls(data);
  data=btopn("AnhViet.dat",0,0);
}

void VietAnh(){
  btcls(data);
  data=btopn("VietAnh.dat",0,0);
}

GtkWidget* gtk_button_with_icon_and_label(char* label,char* fileimage){
  GtkWidget *button,*image;
  image=gtk_image_new();
  gtk_image_set_from_file(GTK_IMAGE(image),fileimage);
  button=gtk_button_new_with_label(label);
  gtk_button_set_image((GtkButton *)button,image);
  gtk_button_set_image_position((GtkButton *)button,GTK_POS_LEFT);
  return button;
}

//Tao icon ghep vao button
GdkPixbuf *create_pixbuf(const gchar *filename){
  GdkPixbuf *pixbuf;
  GError *error=NULL;
  pixbuf=gdk_pixbuf_new_from_file(filename,&error);
  if (!pixbuf){
    fprintf(stderr,"%s\n",error->message);
    g_error_free(error);
  }
  return pixbuf;
}

int main(int argc,char *argv[]){
  GtkWidget *window,*hbox;
  GtkWidget *button,*entry,*label,*title,*table,*wins;
  GtkWidget *ins,*del,*hlp,*info,*clear;
  GtkWidget *halign,*valign;
  // Khoi tao giao dien
  gtk_init(&argc,&argv);

  data=btopn("AnhViet.dat",0,FALSE);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_widget_set_size_request (window, 800, 600);  
  gtk_window_set_title(GTK_WINDOW(window), "Từ điển Anh - Việt");
  gtk_container_set_border_width(GTK_CONTAINER(window), 15);
  gtk_window_set_icon(GTK_WINDOW(window),create_pixbuf("main.png"));
  g_signal_connect(window,"delete-event",G_CALLBACK(delete_event),NULL);
  mainwindow=window;
  // Ket thuc khoi tao


  // Tao bang chung cho tu dien

  table = gtk_table_new(10, 10, FALSE);
  gtk_table_set_col_spacings(GTK_TABLE(table), 3);
  gtk_table_set_row_spacing(GTK_TABLE(table), 0, 3);

  hbox = gtk_hbox_new (GTK_ORIENTATION_HORIZONTAL, 0);

  GtkEntryCompletion *completion=gtk_entry_completion_new();
  gtk_entry_completion_set_text_column(completion,0);
  GtkListStore *liststore=gtk_list_store_new(5,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);
  gtk_entry_completion_set_model(completion,GTK_TREE_MODEL(liststore));

  title = gtk_label_new("Tìm kiếm ");
  gtk_box_pack_start (GTK_BOX (hbox), title, 0, 1, 0);

  entry=gtk_search_entry_new();
  gtk_widget_set_size_request (entry, 400, 30);
  gtk_entry_set_completion(GTK_ENTRY(entry),completion);
  g_signal_connect(entry,"activate",G_CALLBACK(display),NULL);

  g_signal_connect(entry,"key-press-event",G_CALLBACK(searchword),liststore);

  gtk_box_pack_start (GTK_BOX (hbox), entry, 1, 1, 0);

  GtkSettings *default_settings=gtk_settings_get_default();
  g_object_set(default_settings,"gtk-button-images",TRUE,NULL);

  button=gtk_button_with_icon_and_label("","search.png");
  gtk_widget_set_size_request (button, 50, 20);
  g_signal_connect(button,"clicked",G_CALLBACK(search_word),(gpointer)entry);
  gtk_box_pack_start (GTK_BOX (hbox), button, 1, 1, 0);

  gtk_table_attach (GTK_TABLE (table), hbox, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);


  
  // Tao cac button them, xoa, thong tin, khoi phuc

  ins=gtk_button_with_icon_and_label("  Thêm từ","add.png");
  g_signal_connect(ins,"clicked",G_CALLBACK(add_word),(gpointer)entry);
  gtk_widget_set_size_request(ins, 50, 30);
  gtk_table_attach(GTK_TABLE(table), ins, 3, 4, 1, 2, 
      GTK_FILL, GTK_SHRINK, 1, 1);


  valign = gtk_alignment_new(0, 0, 0, 0);
  del=gtk_button_with_icon_and_label("   Xóa từ   ","del.png");
  g_signal_connect(del,"clicked",G_CALLBACK(delete_word),(gpointer)entry);
  gtk_widget_set_size_request(del, 100, 30);
  gtk_container_add(GTK_CONTAINER(valign), del);
  gtk_table_set_row_spacing(GTK_TABLE(table), 1, 2);
  gtk_table_attach(GTK_TABLE(table), valign, 3, 4, 2, 3, 
      GTK_FILL, GTK_FILL | GTK_EXPAND, 1, 1);

  hlp=gtk_button_with_icon_and_label(" Trợ giúp","help.png");
  g_signal_connect(hlp,"clicked",G_CALLBACK(show_guide),(gpointer)entry);
  gtk_widget_set_size_request(hlp, 50, 30);
  gtk_table_attach(GTK_TABLE(table), hlp, 3, 4, 3, 4, 
      GTK_FILL, GTK_SHRINK, 1, 1);

  halign = gtk_alignment_new(0, 1, 0, 0);
  info=gtk_button_with_icon_and_label(" Thông tin","info.png");
  g_signal_connect(info,"clicked",G_CALLBACK(show_info),(gpointer)entry);
  gtk_container_add(GTK_CONTAINER(halign), info);
  gtk_widget_set_size_request(info, 70, 30);
  gtk_table_set_row_spacing(GTK_TABLE(table), 3, 5);
  gtk_table_attach(GTK_TABLE(table), halign, 0, 1, 4, 5, 
      GTK_FILL, GTK_FILL, 0, 0);

  clear=gtk_button_with_icon_and_label(" Khôi phục","clear.png");
  g_signal_connect(clear,"clicked",G_CALLBACK(clearAll),(gpointer)entry);
  gtk_widget_set_size_request(clear, 70, 30);
  gtk_table_attach(GTK_TABLE(table), clear, 3, 4, 4, 5, 
      GTK_FILL, GTK_FILL, 0, 0);
//


  //Tao hop hien nghia

  hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

  GtkWidget* scrolledWindow=gtk_scrolled_window_new(NULL,NULL);
  gtk_container_set_border_width(GTK_CONTAINER (scrolledWindow),10);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

  textView=gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textView), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textView), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView),GTK_WRAP_WORD);
  buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
  gtk_text_buffer_set_text(buffer,"",-1);
  gtk_container_add(GTK_CONTAINER(scrolledWindow),textView);
  gtk_box_pack_start(GTK_BOX(hbox),scrolledWindow,TRUE,TRUE,0);
  gtk_table_attach(GTK_TABLE(table),hbox , 0, 2, 1, 4, 
      GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 1, 1);
  //

  gtk_container_add(GTK_CONTAINER(window), table);

  g_signal_connect(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(window));

  //gtk_container_add(GTK_CONTAINER(window), table);

  gtk_widget_show_all(window);
  gtk_main();
  btcls(data);
  return 0;
}