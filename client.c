#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>
#include <pthread.h> 



extern int errno;
int port;

void urgHandler(int);
void* chat_scrie(int);
void* chat_citeste(int);
//void onlineUser(int);

int meniu2()
{
  int op;
  int i;
  char CHop2[100];
  printf("\n\n\x1b[31m**** Meniu(2) ****\x1b[0m\n");
  while(1)
  {
  	  printf("\n1.Setare Profil\n2.Adauga un prieten\n3.Posteaza o stire\n4.Vizualizare postari prieteni\n5.Chat cu prietenii\n6.Delogare\n");
      printf("op = ");
      bzero(CHop2,sizeof(CHop2));
      scanf("%s",CHop2);

      if(strcmp(CHop2,"1")==0) {op=1; break;} else if(strcmp(CHop2,"2")==0) {op=2; break;} else if(strcmp(CHop2,"3")==0) {op=3; break;} else if(strcmp(CHop2,"4")==0) {op=4; break;} 
      else if(strcmp(CHop2,"5")==0) {op=5; break;} else if(strcmp(CHop2,"6")==0) {op=6; break;}  
      else
        printf("\n>>> Optiune invalida\n\n");
  }

  return op;
}

int meniu()
{
  int op;
  int i;
  char CHop[100];
  printf("\n\n\x1b[31m**** Meniu(1) ****\x1b[0m\n");
  while(1)
  {    
     printf("\n1.Inregistrare\n2.Logare\n3.Vizualizare postari publice\n");
     printf("op = ");
     bzero(&CHop,sizeof(CHop));
     scanf("%s",CHop);
  
    if(strcmp(CHop,"1")==0){op =1; break;}else if(strcmp(CHop,"2")==0){op =2; break;} else if(strcmp(CHop,"3")==0){op =3; break;}
    else
      printf("\n>>> Optiune invalida!\n\n");
  }
  return op;
}

int meniuAdmin()
{
  int op;
  int i;
  char CHop[100];
  printf("\n\n\x1b[31m**** Meniu( \x1b[34m-admin-\x1b[31m ) ****\x1b[0m\n");
  while(1)
  {    
     printf("\n1.Trimite mesaj catre toti utilizatorii de pe server\n2.\n3.\n");
     printf("op = ");
     bzero(&CHop,sizeof(CHop));
     scanf("%s",CHop);
  
    if(strcmp(CHop,"1")==0){op =1; break;}else if(strcmp(CHop,"2")==0){op =2; break;} else if(strcmp(CHop,"3")==0){op =3; break;}
    else
      printf("\n>>> Optiune invalida!\n\n");
  }
  return op;
}

struct str_adminMsg
{
	char user[50];
	char msg[500];
};

struct str_chat2user
{
    char user1[50]; // user1 trimite catre user2 cerere de chat
    char nume_prenume_u1[100];
    char user2[50];
    char nume_prenume_u2[100];
};

struct str_msg_chat
{
	char userr[50];
	char msg[120];
};

 char * concatenare ( char * string1 , char* string2)
 { // functie folosita pentru concatenarea variabilelor 
  //in instructiunile sql
   size_t str1=strlen(string1);
   size_t str2=strlen(string2);
   size_t total =str1+str2;

   char *  destinatie = malloc(total + 1);

   strcpy(destinatie,string1);
   strcpy(destinatie+str1,string2);

   return destinatie;
 }

struct str_login
{
  char user[50] ;
  char pw[50] ;
  char raspuns[50]; // succes||eroare
  int profilul; //0:nu a fost setat 1-public, 2 pentru privat::2-doar prietenii iti pot vedea activitatea altfel toata lumea
  int tip;
};

struct str_adaugaPrieten
{
	char utilizatori_Online[5000];
	char lista_Prieteni[5000];
	char utilizatorii_care_nu_iti_sunt_prieteni[5000];
	char utilizatorii_online_care_nu_iti_sunt_prieteni[5000];
};

struct str_postari
{ 
    char username[50];
    char catre_grupul[20];
    char titlu [500];
    char continut[5000];
};

struct str_prieten
{
  char cod1_uname[50] ; // username
  char cod2_uname[50] ; // username  e cheie primara 
  char tip[15] ;
};


struct register_user
{
  char Newuser[50];
  char pw[50];
  char nume[50];
  char prenume[50];
  int tip; // completam cu 1 mentru admin respectiv 2 pentru utilizator normal 
};

  intptr_t sd;
  struct sockaddr_in server;
  struct str_login login_request;
  struct register_user register_request;
  struct str_prieten prieten_sr;
  struct str_postari postari_request;
  struct str_adaugaPrieten Adauta_un_prieten;
  struct str_chat2user chat2user_request;
  struct str_msg_chat msg_chat_request;
  struct str_adminMsg admin_request;
  int op,op1,length=0;
  bool logat =false; 
  char raspuns[50];
  char msg[50];
  pthread_t thr1,thr2; 

  char msg_copie[49];
char comandaMeniuChat[13];
char getUserName[50];


int main (int argc, char *argv[])
{
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  port = atoi (argv[2]);

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare nr: 1 \n");
      return errno;
    }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);
  
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("Eroare nr: 2\n");
      return errno;
    }

         signal(SIGURG,urgHandler);
     fcntl(sd, F_SETOWN, getpid());


     
   _meniu:
  op=meniu();
  op1=op;

  length=sizeof(server);

  switch(op1) //op1 este valoarea returnata de meniu()
  {
      case 1:
      		printf("\n\x1b[34m--- Inregistrare ---\x1b[0m\n");
            strcpy(raspuns,"Contul nu a fost creat");
            while (1) //atata timp cat raspuns!='Contul a fost creat';
            {

			bzero (&login_request, sizeof(struct str_login));

            //completam structura register_user 
            printf("\nCompletati urmatoarele campuri: \n");
            printf("Username: ");
            scanf("%s",register_request.Newuser);
            printf("Parola: ");
            scanf("%s",register_request.pw);
            printf("Numele: ");
            scanf("%s",register_request.nume);
            printf("Prenumele: ");
            scanf("%s",register_request.prenume);
      choose_tip_user:
            printf("Utilizatorul va fi normal sau administrator? normal/admin: ");
            char tip_user[50];
            scanf("%s",tip_user);

            if(strcmp(tip_user,"normal")==0)
            	register_request.tip=2;
            else
            	if(strcmp(tip_user,"admin")==0)
            		register_request.tip=1;
            	else
            		{printf("\nTipul pe care l-ai introdus este nevalid\n");
            			goto choose_tip_user;}

			// trimitem op1=1 catre server pentru ca acesta sa stie ca 
              //vrem sa cream un nou cont
          if (write (sd, &op, sizeof(int)) <= 0)
          {
            perror ("Eroare nr: 3\n");
            return errno;
          }

            // trimitem structura catre server
        if (write (sd, &register_request, sizeof(struct register_user)) <= 0)
          {
            perror ("Eroare nr: 4\n");
            return errno;
          }

            bzero(&raspuns,sizeof(char)*50); 

            //intram iar intr-o structura blocanta pana primim raspunsul de la server

          if (read (sd, &raspuns, sizeof(char)*50) < 0)
	        {
	          perror ("Eroare nr: 5\n");
	          return errno;
	        }

              printf("\n>>> %s\n\n",raspuns );

                if(strcmp(raspuns,"Contul nu a fost creat")!=0)
                  if(strcmp(raspuns,"Contul exista")!=0)
                  goto _meniu; // daca contul a fost creat iesim din bucla

                char w_op[2];
                sus:
                printf("continui/alta optiune (c/ao) ?\nop: ");
                scanf("%s",w_op);
                if(strcmp(w_op,"c")==0)
                {;}
              else if(strcmp(w_op,"ao")==0)
                  goto _meniu;
                  else
                    {printf("Optiune nevalida\n");
                      goto sus;
                    }
             }

        break;/* end register */

      case 2:
          while(1)
          {
          	looplogin:
          strcpy(login_request.raspuns,"Nelogat"); // initial suntem nelogati pe server

            //completam structura

          printf("\n\x1b[34m--- Logare ---\x1b[0m\n\n");
              printf("Introduceti userul si parola:\n");
              printf("User: ");
              scanf("%s",login_request.user);
              printf("Parola: ");
              scanf("%s",login_request.pw);
                  

//trimitem op1=2 pentru ca acesta sa stie ca urmeaza o incercare de login
         if (write (sd, &op, sizeof(int)) <= 0)
          {
            perror ("Eroare nr: 6\n");
            return errno;
          }
              //trimitem structura catre server
            if (write (sd, &login_request, sizeof(struct str_login)) <= 0)
            {
              perror ("Eroare nr: 7\n");
              return errno;
            }

              //clientul intra iar intr-o structura blocanta pana primeste raspunsul de la server
            bzero (&login_request, sizeof(struct str_login));
               if (read (sd, &login_request, sizeof(struct str_login)) < 0)
            {
              perror ("Eroare nr: 8\n");
              return errno;
            }

            loopLogin2:
                printf("\n>>> Esti %s pe server\n\n",login_request.raspuns ); // afisam raspunsul primit

                  if(strcmp(login_request.raspuns,"Logat")==0)
                    break; //daca raspunsul este "Logat" iesim din bucla
                	else
                	{
                		char schooseOp[65];
                		printf("Incerci iar / alegi alta optiune? (ir/ao)\n");
                		bzero(schooseOp,65);
                		read(0,schooseOp,65);
                		printf("\n%s\n",schooseOp);
                		if((strcmp(schooseOp,"ir\n")==0)||(strcmp(schooseOp,"IR\n")==0))
                			goto looplogin;
                		else
                			if((strcmp("ao\n",schooseOp)==0)||((strcmp("AO\n",schooseOp)==0)))
                				goto _meniu;

                			else
                				printf("\nOptiune gresita\n");
                				goto loopLogin2;


                	}

              } //end while verificare login

              //printf("\nProfilul este %d\n",login_request.tip);

              //printf("\n\n>>>Instructiuni restrictionate de login:\n\n");

                //signal(SIGTSTP, onlineUser); // doar dupa ce ne-am logat vom putea vede cine este online

              int oop;
              int oop_switch;

while(1){
  meniu2_label:

  		if(login_request.tip==2) 
              oop_switch =meniu2()+3;

  		if (login_request.tip==1)
  			oop_switch =meniuAdmin()+11;
  		


             // printf("%d\n",login_request.profilul);

              switch(oop_switch)
              {
                  case 4:
                            printf("\n\x1b[34m--- Setare profil ---\x1b[0m\n\n");
                              switch(login_request.profilul)
                              {
                                case 1:
                                    printf("Profilul este: \x1b[31mPublic\x1b[0m\n");
                                break;

                                case 2:
                                    printf("Profilul este: \x1b[31mPrivat\x1b[0m\n");
                                break;

                                default:
                                    printf("\x1b[31mProfilul nu a fost setat.\x1b[0m\n");
                                break;
                              }
                            msg1_profilll:

                              printf("Doriti sa va setati/schimbati profilu?  (da/nu)\n");
                              char  msg1_profil[11];
                              bzero(&msg1_profil,sizeof(msg1_profil));
                              scanf("%s",msg1_profil);
                              char raspuns_pt_client_interogare[6];

                                  if(strcmp(msg1_profil,"da")==0)
                                  {
                                              ;// trimite mesaj la server
                                              int  val_profil;
                                              char val_prof_char[50];
                                              _val_profil_setat:
                                              printf("Alege 1/2 pentru public/privat:");
                                              fflush(stdout);
                                              bzero(&val_prof_char,sizeof(char)*50);
                                              read(0,val_prof_char,sizeof(char)*50);

                                              if(strcmp(val_prof_char,"1\n")==0)
                                                  val_profil=1;
                                                if(strcmp(val_prof_char,"2\n")==0)
                                                  val_profil=2;

                                                  login_request.profilul=val_profil;
                                                if((val_profil==2)||(val_profil==1))
                                                  {

        			 										                  if (write (sd, &oop_switch, sizeof(int)) <= 0)
        			                                          {
        			                                            perror ("Eroare nr: 9\n");
        			                                            return errno;
        			                                          }

        	                                              if (write (sd, &login_request, sizeof(struct str_login)) <= 0)
        	                                                {
        	                                                  perror ("Eroare nr: 10\n");
        	                                                  return errno;
        	                                                }
        	                                                bzero(&raspuns_pt_client_interogare,sizeof(raspuns_pt_client_interogare));

        	                                                  if (read (sd, &raspuns_pt_client_interogare, sizeof(char)*6) <= 0)
        	                                                {
        	                                                  perror ("Eroare nr: 11\n");
        	                                                  close (sd); 
        	                                                } 


                                        } // primirea raspunsului
                                      else
                                        goto _val_profil_setat;

                                  }else if(strcmp("nu",msg1_profil)==0)
                                  {;}
                                  else
                                    {printf("Mesaj gresit, incearca iar!\n");
                                    goto msg1_profilll;}

                                    printf("\n>>> Mesajul primit: %s\n\n", raspuns_pt_client_interogare);

												                                 /* if(strcmp(raspuns_pt_client_interogare,"Succes")==0)
												                                    printf("Am realizat cu succes modificarea.\n");
												                                  else
												                                    printf("Modificarea a esuat\n");
												                                    */
                
                  break;  /* end modificare profil*/

                  case 5:
                      printf("\n\n\x1b[34m--- Adauga un prieten ---\x1b[0m\n\n");
                                        if (write (sd, &oop_switch, sizeof(int)) <= 0)
                                          {
                                            perror ("Eroare nr: 12\n");
                                            return errno;
                                          } 

                                              if (write (sd, &login_request, sizeof(struct str_login)) <= 0)
                                                {
                                                  perror ("Eroare nr: 13\n");
                                                  return errno;
                                                } 

                                                bzero(&Adauta_un_prieten,sizeof(Adauta_un_prieten));
                                     if (read (sd, &Adauta_un_prieten, sizeof(struct str_adaugaPrieten)) <= 0)
                                    {
                                      perror ("Eroare nr: 14\n");
                                      close (sd); 
                                    } 
                                     printf("Lista prieteni: %s\n",Adauta_un_prieten.lista_Prieteni);
                                 	 sleep(1);
                                     printf("Utilizatorii logati pe server sunt: %s \n",Adauta_un_prieten.utilizatori_Online);
                                     sleep(1);
                                     printf("Lista utilizatorilor care nu iti sunt prieteni:%s\n",Adauta_un_prieten.utilizatorii_care_nu_iti_sunt_prieteni);
                                     sleep(1);
                                     printf("Lista utilizatorilor logati care nu iti sunt prieteni: %s\n",Adauta_un_prieten.utilizatorii_online_care_nu_iti_sunt_prieteni);

                                    char add_prieten[20];
                                    char tip_add_prieten[20];
                                    char intrebare_add_prieten[10];
                                    char intrebare_add_prieten2[10];
                                    char raspuns_catre_client_add_prieten[100];

                                      printf("Introdu numele celui pe care vrei sa il adaugi: ");
                                      bzero(&add_prieten,sizeof(add_prieten));
                                      scanf("%s",add_prieten);
                                      printf("Ai introdus: %s\n",add_prieten);

                                      add_prieten_tip:
                                      printf("Introdu tipul prietenului: familie/apropiat/cunostinta/coleg \n");
                                      bzero(&tip_add_prieten,sizeof(tip_add_prieten));
                                      scanf("%s",tip_add_prieten);
                                      if((strcmp(tip_add_prieten,"FAMILIE")==0)||(strcmp(tip_add_prieten,"APROPIAT")==0)||(strcmp(tip_add_prieten,"CUNOSTINTA")==0)||(strcmp(tip_add_prieten,"COLEG")==0)||(strcmp(tip_add_prieten,"familie")==0)||(strcmp(tip_add_prieten,"apropiat")==0)||(strcmp(tip_add_prieten,"cunostinta")==0)||(strcmp(tip_add_prieten,"coleg")==0))
                                        {;}
                                      else
                                        goto add_prieten_tip;
                                    bzero(&prieten_sr,sizeof(prieten_sr));
                                        
                                        strcpy(prieten_sr.cod1_uname,login_request.user);
                                        strcpy(prieten_sr.cod2_uname,add_prieten);
                                        strcpy(prieten_sr.tip,tip_add_prieten);
                                        //printf("Trimitem catre server: %s %s %s\n",prieten_sr.cod1_uname,prieten_sr.cod2_uname,prieten_sr.tip);
                                           
                                            if (write (sd, &prieten_sr, sizeof(struct str_prieten)) <= 0)
                                                      {
                                                        perror ("Eroare nr: 15\n");
                                                        return errno;
                                                      }  

                                                      bzero(&raspuns_catre_client_add_prieten,sizeof(raspuns_catre_client_add_prieten));

                                                        if (read (sd, &raspuns_catre_client_add_prieten, sizeof(raspuns_catre_client_add_prieten)) <= 0)
		                                                    {
		                                                      perror ("Eroare nr: 16\n");
		                                                      return errno;
		                                                    }
                                              printf("\n>>>%s\n\n",raspuns_catre_client_add_prieten);

                  break;

                  case 6:
                      printf("\n\x1b[34m--- Posteaza o stire ---\x1b[0m \n\n");
                      	//cand citim cu read(0,destinatie,sizeof) in destinatie vom avea stringul citit + '\n'
                      	//din cauza acelui '\n' din coada vom aveam problema in parteaza de server cand facem concatenarea
                      	// in partea de mai jos, in for-uri, vom scapa de acel '\n'
                                              
                                          char titluPostare[500];
                                          char titluPostareCopie[500];
                                          int i,lungime;
                                          
                                          bzero (titluPostare, sizeof(char)*500);
                                          printf ("Titlul: "); bzero(titluPostareCopie,sizeof(char)*500);
                                          fflush (stdout);
                                          read (0, titluPostare, 500);
                                          lungime=strlen(titluPostare);
                                          for(i=0;i<lungime-1;i++)
                                            titluPostareCopie[i]=titluPostare[i];

                                          char continutPostare[5000];
                                          char continutPostareCopie[5000];
                                          bzero (continutPostare, sizeof(char)*5000);  bzero(continutPostareCopie,sizeof(char)*5000);
                                          printf ("Continut: ");
                                          fflush (stdout);
                                          read (0, continutPostare, 5000);

                                          lungime=strlen(continutPostare);
                                          for(i=0;i<lungime-1;i++)
                                          continutPostareCopie[i]=continutPostare[i];

                                          char catre_tipPostare[50];
                                          char catre_tipPostareCopie[50];

label_tipPostare:                                          
                                          bzero (catre_tipPostare, sizeof(char)*50);  bzero(catre_tipPostareCopie,sizeof(char)*50);
                                          printf("Catre(familie, public,coleg): ");
                                          fflush (stdout);
                                          read (0, catre_tipPostare, 50);
                                          lungime=strlen(catre_tipPostare);
                                          for(i=0;i<lungime-1;i++)
                                          catre_tipPostareCopie[i]=catre_tipPostare[i];

                                      bzero(&postari_request,sizeof(postari_request));
                                        
                                        if ((strcmp(catre_tipPostare,"familie\n")==0)||(strcmp(catre_tipPostare,"public\n")==0)||(strcmp(catre_tipPostare,"coleg\n")==0))
                                        {
                                          ;
                                          strcpy(postari_request.username,login_request.user);
                                          strcpy(postari_request.titlu,titluPostareCopie);
                                          strcpy(postari_request.catre_grupul,catre_tipPostareCopie);
                                          strcpy(postari_request.continut,continutPostareCopie);

                                         // printf("%s%s%s%s",postari_request.username,postari_request.titlu,postari_request.catre_grupul,postari_request.continut);
                                        }
                                        else
                                          goto label_tipPostare;

                                       if (write (sd, &oop_switch, sizeof(int)) <= 0)
                                                  {
                                                    perror ("Eroare nr: 17\n");
                                                    return errno;
                                                  }  

                                        if (write (sd, &postari_request, sizeof(struct str_postari)) <= 0)
                                          {
                                            perror ("Eroare nr: 18\n");
                                            return errno;
                                          }  

                                          char raspuns_catre_client_postare[50];
                                          bzero(&raspuns_catre_client_postare,sizeof(raspuns_catre_client_postare));

                                          if (read (sd, &raspuns_catre_client_postare, sizeof(char)*50) <= 0)
                                          {
                                            perror ("Eroare nr: 19\n");
                                            return errno;
                                          }
                                          else
                                          {
                                          	printf("\n>>> Mesajul de la server: %s\n",raspuns_catre_client_postare );
                                          } 


                  break;



                  case 7:
                  			printf("\n\x1b[34m--- Vizualizare postari prieteni ---\x1b[0m\n\n");
 											 if (write (sd, &oop_switch, sizeof( int)) <= 0)
	                                            {
	                                              perror ("Eroare nr: 20\n");
	                                              return errno;
	                                            } 

	                                            if (write (sd, &login_request, sizeof( struct str_login)) <= 0)
	                                            {
	                                              perror ("Eroare nr: 21\n");
	                                              return errno;
	                                            } 
	                                            
                	char postariPrieteniContinut[871];//marimea (50+20+100+500+200caracterele speciale)*sizeof(char)+1

                 							while(1)
                 									{
                 										bzero(postariPrieteniContinut,sizeof(postariPrieteniContinut));
						                           	       if (read (sd, &postariPrieteniContinut, sizeof(char)*871) <= 0)
				                                                {
				                                                  perror ("Eroare nr: 22\n");
				                                                  return errno;
				                                                }  

				                                                if(strcmp(postariPrieteniContinut,"gata")==0) // nu mai e nimic de citit de la server
				                                                	break;

				                                                printf("%s\n", postariPrieteniContinut);
				                                                sleep(1);
	                                           		 }


                  break;

                  case 8:
                      printf("\n\n\x1b[34m--- Modul chat ---\x1b[0m\n\n");
                      					printf(" // comentariu Apasa CTRL + z pentru a vedea cine este online. (idee abandonata, comentariile sunt in surse)\n\n");
                      					char userNameChat[51];
                      					char userNameChatCopie[50];
                      					char userCorectChatConfirmare[50];
                      					
                      					int lungimee;

               __confirmareUserChar:

                      					while(1)
                      					{
                                  bzero (userNameChat, 100);
                      						printf("Introdu numele celui cu care vrei sa comunici\n");
                      						printf("username: ");
                      						fflush(stdout);
                      						read(0,userNameChat,50);


										         lungimee=strlen(userNameChat);
                                          bzero(userNameChatCopie,sizeof(userNameChatCopie));
                                          for(i=0;i<lungimee-1;i++)
                                            userNameChatCopie[i]=userNameChat[i];  // eliminam '\n'


                                          bzero(&chat2user_request,sizeof(chat2user_request));
                                          strcpy(chat2user_request.user1,login_request.user);
                                          strcpy(chat2user_request.user2,userNameChatCopie);

                                          if(strcmp(userNameChatCopie,login_request.user)==0)
                                          {
                                            printf("\n\n>>> User nepermis\n\n");
                                            goto __confirmareUserChar;
                                          }

                                        if (write (sd, &oop_switch, sizeof(int)) <= 0)
                                        {
                                          perror ("Eroare nr: 23\n");
                                          return errno;
                                        }

                                        if (write (sd, &chat2user_request, sizeof(chat2user_request)) <= 0)
                                          {
                                            perror ("Eroare nr: 24\n");
                                            return errno;
                                          }

                                          bzero(userCorectChatConfirmare,sizeof(char)*50);

		                                  if (read (sd, &userCorectChatConfirmare, sizeof(char)*50) <= 0)
		                                    {
		                                      perror ("Eroare nr: 25\n");
		                                      return errno;
		                                    } 



		                                    if(strcmp(userCorectChatConfirmare,"User gresit")==0)
		                                    {
                                          sch_op:
				                                    	printf("\n>>> Userul introdus este gresit sau nu este online\nIncerci iar/schimbi optiunea? ir/sc\n");
		                                          char schimba_op[40];
		                                          bzero(&schimba_op,sizeof(schimba_op));
		                                          read(0,&schimba_op,sizeof(char)*40);
		                                          if((strcmp(schimba_op,"ir\n")==0)||(strcmp(schimba_op,"IR\n")==0))
				                                    	goto __confirmareUserChar;
		                                        else
		                                          if((strcmp(schimba_op,"sc\n")==0)||(strcmp(schimba_op,"SC\n")==0))
		                                            goto meniu2_label;
		                                          else
		                                            {printf("\nOptiune gresita");
		                                              goto sch_op;
	                                            	}
		                                    }
		                                    else if (strcmp(userCorectChatConfirmare,"User chat ocupat")==0)
		                                    {
		                                    	printf("\n>>> Userul introdus este deja in chat.\n>>> Trebui sa primiti o invitatie de chat de la el pentru a putea conversa\n");
		                                    	fflush(stdout);
		                                    	goto meniu2_label;
		                                    }
		                                    else
		                                    {
		                                    	
		                                    	printf("\n\n>>> Userul introdus este valid\nI-am trimis cererea de chat\n");

		                                    	//creem 2 fire de exec
		          pthread_create(&thr2,NULL,(void *)chat_scrie,(void *)sd);
		          pthread_create(&thr1,NULL,(void *)chat_citeste,(void *)sd);
		              pthread_join(thr2,NULL);  
		              pthread_join(thr1,NULL); 


                                          		break ;
		                                    }

                      					} // end while

					 break;



                  case 9:
                      printf("\n\x1b[34m--- Delogare ---\x1b[0m\n\n");

                                          char delogare_q[10];
                                          delogare_eticheta:
                                          printf("Sunteti sigur ca vreti sa va delogati? (da/nu)\n");
                                          printf("Ai ales: ");
                                          scanf("%s",delogare_q);
                                          if((strcmp("da",delogare_q)==0)||(strcmp("DA",delogare_q)==0))
                                          {
                                       
                                        if (write (sd, &oop_switch, sizeof(int)) <= 0)
                                          {
                                            perror ("Eroare nr: 26\n");
                                            return errno;
                                          }

                                          // trimitem datele de logare
                                              if (write (sd, &login_request, sizeof(struct str_login)) <= 0)
                                                {
                                                  perror ("Eroare nr: 27\n");
                                                  return errno;
                                                }  
                                                bzero(&login_request,sizeof(login_request));
                                             if (read (sd, &login_request, sizeof(struct str_login)) <= 0)
                                                {
                                                  perror ("Eroare nr: 28\n");
                                                  close (sd); 
                                                }                                         
                                          }
                                          else
                                            if((strcmp("nu",delogare_q)==0)||(strcmp("NU",delogare_q)==0))
                                              {;
                                                goto meniu2_label;
                                              }
                                            else
	                                            {
	                                              printf("Comanda gresita\n");
	                                              goto delogare_eticheta;
	                                            }

                                            if(strcmp("Nelogat",login_request.raspuns)==0)
                                            {
                                              printf("\n>>> Ai reusit sa te deloghezi de pe server\n");
                                            }
                                            goto _meniu; // nu mai avem acces la instructiunile restrictionate de login

                  break;


                  case 12: 
                  			printf("\n\x1b[34m--- [admin] Trimite un mesaj catre toti utilizatorii de pe servar ---\x1b[0m\n\n");
                                        if (write (sd, &oop_switch, sizeof(int)) <= 0)
                                          {
                                            perror ("Eroare nr: 29\n");
                                            return errno;
                                          }
                                          char msg_admin[500];
                                          char msg_adminCopie[500];
                                          int lzz,lz;
                                          bzero(&msg_admin,sizeof(char)*500);
                                          printf("\nIntrodu mesajul\nADMIN:");
                                          fflush(stdout);
                                          read(0,msg_admin,sizeof(char)*500);

                                          lzz=strlen(msg_admin);
                                          bzero(&admin_request,sizeof(struct str_adminMsg));
                                          for(lz=0;lz<lzz-1;lz++)
                                          	msg_adminCopie[lz]=msg_admin[lz];

                                          strcpy(admin_request.msg,msg_adminCopie);
                                          strcpy(admin_request.user,login_request.user);


                                          if (write (sd, &admin_request, sizeof(struct str_adminMsg)) <= 0)
                                          {
                                            perror ("Eroare nr: 30\n");
                                            return errno;
                                          }


                  break;		

              }
         }   //end while bucla pentru instructiunile din meniu2()     
          
          break; /*end login */

      case 3:
              //vizualizarea postarilor publice
      				printf("\n\x1b[34m--- Vizualizare postarilor publice ---\x1b[0m\n\n");
                                              if (write (sd, &op, sizeof( int)) <= 0)
                                                {
                                                  perror ("Eroare nr: 31\n");
                                                  return errno;
                                                }  
                 	char postarePublicaContinutCatreClient[871];//marimea (50+20+100+500+200caracterele speciale)*sizeof(char)+1

                 							while(1)
                 									{

                 										bzero(postarePublicaContinutCatreClient,sizeof(postarePublicaContinutCatreClient));
						                           	       if (read (sd, &postarePublicaContinutCatreClient, sizeof(char)*871) <= 0)
				                                                {
				                                                  perror ("Eroare nr: 32\n");
				                                                  return errno;
				                                                }  

				                                                if(strcmp(postarePublicaContinutCatreClient,"gata")==0) // nu mai e nimic de citit de la server
				                                                	break;

				                                                printf("%s\n", postarePublicaContinutCatreClient);
				                                                sleep(1);
	                                           		 }

                 goto _meniu;                           	    	
        break;

       // default:
               // break;
  }
  close (sd);
}



void* chat_scrie(int sd)  
 {  
 	/*            
 			  int aasd=10;
 	          write(sd,&aasd,sizeof(int));
 	          char * mesaj_bun_veni_pe_chat="Utilizatorul ";
 	          char mesaj_bun_veni_pe_chat_copie[120];

 	          mesaj_bun_veni_pe_chat=concatenare(mesaj_bun_veni_pe_chat,login_request.user);
 	          mesaj_bun_veni_pe_chat=concatenare(mesaj_bun_veni_pe_chat," s-a conectat la chat");
 	          strcpy(mesaj_bun_veni_pe_chat_copie,mesaj_bun_veni_pe_chat);

              write(sd,&mesaj_bun_veni_pe_chat_copie,sizeof(char)*120);
*/


      while(1)  
      { 
              int asdasda=10;
    afara_meniu_chat:
              
              printf("\x1b[0m\x1b[36m%s:\x1b[0m ",login_request.user);
                fflush(stdout);
                bzero(msg,sizeof(char)*50);
              if(read(0,msg,50)<0){printf("\nEroare nr: 33");}

              if(strcmp(msg,"\n")!=0) // nu vrem sa trimitem catre server '\n', un utilizator poate apasa involuntar "ENTER"
              {
              	
              	bzero(&msg_copie,sizeof(char)*49);
              	int lung_msg,i_lung_msg;
              	lung_msg=strlen(msg);
              	for(i_lung_msg=0;i_lung_msg<lung_msg-1;i_lung_msg++)
              		msg_copie[i_lung_msg]=msg[i_lung_msg];
                
                bzero(&msg_chat_request,sizeof(msg_chat_request));
              	strcpy(msg_chat_request.userr,login_request.user);
              	strcpy(msg_chat_request.msg,msg_copie);

              	
                bzero(&comandaMeniuChat,sizeof(char)*13);
              	for(i_lung_msg=0;i_lung_msg<13;i_lung_msg++)
              		comandaMeniuChat[i_lung_msg]=msg[i_lung_msg];

              	if((strcmp(msg_chat_request.msg,"/help")==0)||(strcmp(msg_chat_request.msg,"/h")==0)){printf("Meniu chat: \n-adaugaPrieten\n-quit | exit\n"); goto afara_meniu_chat;}
              		
                  if(strcmp(comandaMeniuChat,"adaugaPrieten")==0)
              		{              				
              				int chatConv =11;
                      pthread_cancel(thr1); //omoram firul pentru a nu citi mesajul de la server prin functia chat_citeste, si a trata gresit raspunsul
                      char raspuns_getUserName[50];
                      bzero(&getUserName,sizeof(getUserName));
              				for(i_lung_msg=0;i_lung_msg<lung_msg-15;i_lung_msg++)
              					if(getUserName[i_lung_msg]!='\n')
              					getUserName[i_lung_msg]=msg[i_lung_msg+14];

              				if(strcmp(getUserName,login_request.user)!=0)
              				{

												  if(write(sd,&chatConv,sizeof(int))<0){printf("\nEroare nr: 34");}

										              strcpy(chat2user_request.user1,login_request.user);
						  						     if (write (sd, &chat2user_request, sizeof(chat2user_request)) <= 0){perror ("Eroare nr: 35\n"); }
										             if(write(sd,&getUserName,sizeof(getUserName))<0){printf("\nEroare nr: 36");}

			                           bzero(&raspuns_getUserName,sizeof(raspuns_getUserName));
			                           if (read (sd, raspuns_getUserName, sizeof(raspuns_getUserName)) <= 0){puts ("Eroare nr: 37\n");}

			                                      pthread_create(&thr1,NULL,(void *)chat_citeste,(void *)(intptr_t)sd); //dupa ce am citit mesajul creem din nou threadul omorat mai sus cu 12 linii de cod

			                                      if(strcmp("gresit",raspuns_getUserName)==0)
			                                      printf("\nUserul pe care ai incercat sa il adaugi la chat este: %s ori nu este online\n",raspuns_getUserName);
			                                      fflush(stdout);
                              }
                                else
                                printf("\nNu te poti adauga pe tine la chat\n")   ;   


              		}
              else
                  	if((strcmp(msg_chat_request.msg,"quit")==0)||(strcmp(msg_chat_request.msg,"exit")==0))
                  	{
                  		printf("\n\n\x1b[32mAi iesit din chat\x1b[0m\n");

    						          if(write(sd,&asdasda,sizeof(int))<0){printf("\nEroare nr: 38");}
    				              if(write(sd,&msg_chat_request,sizeof(struct str_msg_chat))<0){printf("\nEroare nr: 39");}

    				              pthread_cancel(thr2);
    				              pthread_cancel(thr1);
    				              sleep(1);  // functia pthread_cancel() este mai lenta decat structura while(1) 
    				              			//ar aparea un mesaj in plus pe chat daca nu am astepta o sec.
    				              			// un mesaj de citire fara "actiune"
                  	}
              	else 
                  	{
    				             if(write(sd,&asdasda,sizeof(int))<0){printf("\nEroare nr: 40");}
                          int lung;
                          lung=strlen(msg_chat_request.msg);
    				              if(write(sd,&msg_chat_request,sizeof(struct str_msg_chat))<0){printf("\nEroare nr: 41");}
                  	}
			         	 

          	  }
      }// end while 1
 }



  void* chat_citeste(int sd)  // citim si afisam mesajul, aceasta functie o folosim la comunicarea dintre 2 sau mai multi utilizatori
 {
    while(1)
    {

      if (read (sd, &msg_chat_request, sizeof(struct str_msg_chat)) <= 0)
        {
          perror ("\n\nEroare nr:42\nA picat servarul\n\n");
         exit(0);
        }

        if(strcmp(msg_chat_request.msg,"\n")!=0) // nu vrem sa afisam mesajele de tipul '\n' fiindaca utilizatorul poate apasa involuntar '\n'
      		printf ("\n%s",msg_chat_request.msg); 
      	fflush(stdout);
     
      bzero (&msg_chat_request, sizeof(msg_chat_request));
    }
 } 



void urgHandler(int signnr)
{
	
	int n;
	char buff;
	n=recv(sd,&buff, sizeof(buff), MSG_OOB);
	int d;
    d=(int)buff;
    char  user_online[67];
    char admin_online[50];

        switch(buff)
        {
              /*	case 'z':
                      	sleep(1);
                      				printf("\n\n\n>>> Utilizatorii online sunt: ");
              	        			while(1)
              	        			{
              	        							bzero(&user_online,sizeof(user_online));
                                                    if (read (sd, &user_online, sizeof(char)*67) <= 0)
                                                      {
                                                        perror ("Eroare nr: 42\n");
                                                      } 
                                                      
                                                      if(strcmp("gata",user_online)==0)
                                                      	break;

                                                      printf("\x1b[35m\n%s\x1b[0m",user_online);
                                                      sleep(1);
              	        			}
              	        			puts("\n");

               break;
               */

          case 'c':
                    printf("\n\n>>> Ai primit cerere de chat\n\n");

                                if (read (sd, &chat2user_request, sizeof(chat2user_request)) <= 0)
                                  {
                                    perror ("Eroare nr: 43\n");
                                  }


                                  printf("\n\x1b[33mAi primit cerere de chat de la \x1b[31m%s\x1b[33m\n",chat2user_request.user1);

                           pthread_create(&thr2,NULL,(void *)chat_scrie,(void *)sd);
                           pthread_create(&thr1,NULL,(void *)chat_citeste,(void *)sd);

                           pthread_join(thr2,NULL);  
                           pthread_join(thr1,NULL); 

          break;

          case 'p': 
                   printf("\n\n>>> O noua postare pe server\n");
                   char stirePregatire[1000];

                   bzero(&stirePregatire, sizeof(stirePregatire));
                    if (read (sd, &stirePregatire, sizeof(char)*1000) <= 0)
                          {
                            perror ("Eroare nr: 44\n");
                          } 

                          printf("%s\n",stirePregatire);
                          fflush(stdout);

          break;

          case 'o': // mesaj cand se conecteaza un administrator
          				bzero(&admin_online,sizeof(char)*50);

          				if(read(sd,admin_online,sizeof(char)*50)<0) {printf("\nEroare nr: 45\n");}
          				else
          					{printf("\n\n\x1b[32m>>> S-a conectat la servar administratorul \x1b[34m%s\x1b[32m <<<\x1b[0m\n",admin_online);
          						fflush(stdout);}
          break;

          case 't': //mesaj de l admin
          			
          			bzero(&admin_request,sizeof(struct str_adminMsg));
          			if(read(sd,&admin_request,sizeof(struct str_adminMsg))<0){printf("\nEroare nr: 46\n");}
          			else
          			{
          				printf("\n\nMesaj de la [\x1b[34m%s\x1b[0m]:[\x1b[34m%s\x1b[0m] \n\n",admin_request.user,admin_request.msg);
          				fflush(stdout);
          			}

          break;
        }

bzero(&buff,sizeof(buff));
}

/*  Ideea de a vedea la orice moment cine este online functioneaza foarte bine in felul acesta cand folosim servere concurente create prin fork().
	Dezavantajul servarelor concurente de tip fork() este ca nu pot comunica clientii cei mai "vechi" cu cei mai "noi" , adica daca clientul A si B se conecteza 
	in urmatoarea ordine A,B atunci B poate trimite mesaje catre A, insa A nu pentru ca nu ii vede descriptorul lui B. (cu fork() facem o copie,si nu copie+actualizare(descriptori))

void onlineUser(int signal_no) {
	printf("\nUtilizatori online\n");

	  send(sd,"z",1,MSG_OOB); 
	  

      if (write (sd, &login_request, sizeof(struct str_login)) <= 0)
        {
          perror ("Eroare nr: 77\n");
        } 

}

*/