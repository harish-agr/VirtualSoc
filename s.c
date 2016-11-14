#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <mysql.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>
#include <netdb.h>
#include <pthread.h> 


//void semnale(int);
void* servarul(int);


#define PORT 2021
extern int errno;


struct str_adaugaPrieten
{
	char utilizatori_Online[5000];
	char lista_Prieteni[5000];
	char utilizatorii_care_nu_iti_sunt_prieteni[5000];
	char utilizatorii_online_care_nu_iti_sunt_prieteni[5000];
};

struct str_login
{
  char user[50] ;
  char pw[50] ;
  char raspuns[50]; // succes||eroare
  int profilul; // 1-public, 2 pentru privat::2-doar prietenii iti pot vedea activitatea altfel toata lumea
  int tip; //1:admin - 2:utilizator normal
};

struct str_adminMsg
{
	char user[50];
	char msg[500];
};

struct str_prieten
{
	char cod1_uname[50] ; // username
	char cod2_uname[50] ; // username  e cheie primara 
	char tip[15] ;
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


struct str_register
{
  char Newuser[50];
  char pw[50];
  char nume[50];
  char prenume[50];
  int tip; // 1/2 admin/normal
};

struct str_postari
{ 
    char username[50];
    char catre_grupul[20];
    char titlu [500];
    char continut[5000];
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

// completam structura bazei de date
   MYSQL *conn;
   MYSQL_RES *res,*lista_u_online,*lista_u_online2,*lista_u_prieteni,*lista_u_nu_esti_prieten,*lista_uo_nu_esti_prieten,*postari_publice_pointer,*postariPrieteniPointer,*postChatVerificare,*numeU1_get,*get_Desc_chat_grup,*chat_verificare_u_group,*Trimit_catre_Public,*Trimit_catre_DualPublic,*admin_online_msg,*admin_send_msg;
   MYSQL_ROW row;
   char *serverDB = "fenrir.info.uaic.ro";
   char *user = "proiectRC";
   char *password = "msoZe0xODR";
   char *database = "proiectRC";

//structurile folosite
  struct sockaddr_in server;  
  struct sockaddr_in from;         
  struct str_login rasp_login;
  struct str_register rasp_registe;
  struct str_prieten prieten_sr;
  struct str_postari postari_request;
  struct str_adaugaPrieten Adauta_un_prieten;
  struct str_chat2user chat2user_request;
  struct str_msg_chat msg_chat_request;
  struct str_adminMsg admin_request;
  int op;      
  int sd;   
  char raspuns[50]; 
  intptr_t client1;
    pthread_mutex_t mlock1=PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mlock2=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mlock3=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mlock4=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mlock5=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mlock6=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mlock7=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mlock8=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mlock9=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mlock10=PTHREAD_MUTEX_INITIALIZER;

	pthread_t  thr[100];  
	int df=0;


int main ()
{  
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare nr: 1\n");
      return errno;
    }


  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
    server.sin_family = AF_INET;  
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);

  int optval=1;
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("Eroare nr: 2\n");
      return errno;
    }
    else
    	printf("\nSucces -bind-");

  if (listen (sd, 5) == -1)
    {
      perror ("Eroare nr: 3\n");
      return errno;
    }
    else
    	printf("\nSucces -listen-");


/* Ne conectam la baza de date */
   conn = mysql_init(NULL);

   if (!mysql_real_connect(conn, serverDB,user, password, database, 0, NULL, 0)) 
   {
      fprintf(stderr, "\nEroare nr: 4\n%s\n", mysql_error(conn));
   }
   else
   	printf("\nSucces -databases-");

  // DELETE FROM `utilizatori_online`
        if (mysql_query(conn, "DELETE FROM `utilizatori_online`")) // aplicam interogarea asupra bazei de date
                {
                      fprintf(stderr, "\nEroare nr: 5\n%s\n", mysql_error(conn));
                }

               if (mysql_query(conn, "DELETE FROM `chat_groups`")) // aplicam interogarea asupra bazei de date
                {
                      fprintf(stderr, "\nEroare nr: 6\n%s\n", mysql_error(conn));
                }
                if (mysql_query(conn, "DELETE FROM `descriptori_utilizatori_logati_pe_sv`")) // aplicam interogarea asupra bazei de date
                {
                      fprintf(stderr, "\nEroare nr: 7\n%s\n", mysql_error(conn));
                }


  while (1) 
 	{
      
      int length = sizeof (from);
      printf ("\n\n[server]Asteptam la portul %d...\n\n",PORT);
      fflush (stdout);

      client1 = accept (sd, (struct sockaddr *) &from, &length);

    pthread_create(&thr[++df],NULL,(void *)servarul,(void *)client1);     
    pthread_detach(thr[df]); 



    } 
}    


void* servarul(int client )
{


	printf("\nId-ul threadului este %d\n",(int)pthread_self());
    // signal(SIGURG,semnale);
    // fcntl(client, F_SETOWN, getpid());

	//INSERT INTO `descriptori_utilizatori_logati_pe_sv` VALUES $client

		char * inter_desc_b="INSERT INTO `descriptori_utilizatori_logati_pe_sv` VALUES (";
		char txtw[16];
		sprintf(txtw, "%d", client);
		inter_desc_b=concatenare(inter_desc_b,txtw);
		inter_desc_b=concatenare(inter_desc_b,");");

		 if (mysql_query(conn, inter_desc_b)) // aplicam interogarea asupra bazei de date
                {
                      fprintf(stderr, "\nEroare nr: 8\n%s\n", mysql_error(conn));
                }

   
		      if (client < 0){perror ("Eroare nr: 9\n");}
		    		printf ("\x1b[33m[sock:%d]\x1b[0m S-a conectat la server...\n", client);


						while(1) //while2
						{
						      bzero(&op,sizeof(int));
						   if (read (client, &op, sizeof(int)) <= 0)
						  {
						    perror ("Eroare nr: 10\n");
						    printf ("\x1b[33m[sock:%d]\x1b[0m S-a deconectat de la server...\n", client);

									    //DELETE FROM `utilizatori_online` WHERE `sd_client` = 6;
									    char txt_dec[16];
									    sprintf(txt_dec, "%d", client);

									    char * interogare_deconectare_eronata="DELETE FROM `utilizatori_online` WHERE `sd_client` =";
									    interogare_deconectare_eronata=concatenare(interogare_deconectare_eronata,txt_dec);
									    interogare_deconectare_eronata=concatenare(interogare_deconectare_eronata,";");
									    mysql_query(conn, interogare_deconectare_eronata); // daca un client este logat si isi incheie activitatea print ctrl+c
									    													// trebuie sters din lista celor logati
									    
									    //DELETE FROM `descriptori_utilizatori_logati_pe_sv` WHERE `id`= $client
									    char * interogare_deconectare_eronata2="DELETE FROM `descriptori_utilizatori_logati_pe_sv` WHERE `id`= ";
									    interogare_deconectare_eronata2=concatenare(interogare_deconectare_eronata2,txt_dec);
									    mysql_query(conn, interogare_deconectare_eronata2);
			                                    
						    close (client); 
						    break;    
						  }


						      switch(op)
						      {
						          case 1:
						         		 pthread_mutex_lock(&mlock2);
											                          printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== Inregistrare ===\n",client);

											                        	bzero(&rasp_registe,sizeof(rasp_registe));  
											                       if (read (client, &rasp_registe, sizeof(struct str_register)) <= 0)
												                      {
												                        perror ("Eroare nr: 11\n");
												                        close (client); 
												                        continue;   
												                      }

											                      printf("\x1b[33m[sock:%d]\x1b[0mDatele primite: \n\tusername: %s\n\tparola: %s\n\tnume: %s\n\tprenume: %s\n",client,rasp_registe.Newuser,rasp_registe.pw,rasp_registe.nume,rasp_registe.prenume);
											                      char * interogare ="SELECT * FROM UTILIZATORI";
											                           
											                          // verificam daca contul exista in baza de date
											                            if (mysql_query(conn, interogare)) {
											                                 fprintf(stderr, "\nEroare nr: 12\n%s\n", mysql_error(conn));
											                             }

											                            res = mysql_use_result(conn);

											                            bool se_poate_crea_contul =true; // contul acesta nu exista in baza de date

											                            while ((row = mysql_fetch_row(res)) != NULL)
											                             {
											                                 if((strcmp(row[0],rasp_registe.Newuser)==0))
											                                    se_poate_crea_contul=false;
											                             }

											                              if(se_poate_crea_contul)
											                             printf("\x1b[33m[sock:%d]\x1b[0mSe poate crea contul\n",client);
											                           else
											                           {
											                           	bzero(&raspuns,sizeof(raspuns));
											                            printf("\x1b[33m[sock:%d]\x1b[0mContul exista\n",client);
											                            strcpy(raspuns,"Contul exista");
											                          }

											                            //INSERT INTO `proiectRC`.`UTILIZATORI`  VALUES ('username', 'parola', 'nume prenume', '2',NULL);
											                          //INSERT INTO `proiectRC`.`UTILIZATORI`  VALUES ('stefanucaUser', 'stefanucaParola', 'Stefanuca Ionel', '2',NULL);
											                          // 2 va fi pentru utilizator normal si NULL pentru ca inca nu mi-am setat profilul: public sau privat
											                            interogare="INSERT INTO `proiectRC`.`UTILIZATORI`  VALUES ('";
											                            interogare=concatenare(interogare,rasp_registe.Newuser);
											                            interogare=concatenare(interogare,"','");
											                            interogare=concatenare(interogare,rasp_registe.pw);
											                            interogare=concatenare(interogare,"','");
											                            interogare=concatenare(interogare,rasp_registe.nume);
											                            interogare=concatenare(interogare," ");
											                            interogare=concatenare(interogare,rasp_registe.prenume);
											                           // interogare=concatenare(interogare,"',2,3)"); //profilul nu a fost setat
											                            interogare=concatenare(interogare,"',");
											              char txtq[16];
										                  sprintf(txtq, "%d", rasp_registe.tip);
											                            interogare=concatenare(interogare,txtq);
											                            interogare=concatenare(interogare,",3)");


											             

											                            printf("\x1b[33m[sock:%d]\x1b[0m%s\n",client,interogare);
											                            if (mysql_query(conn, interogare)) // aplicam interogarea asupra bazei de date
											                            {
											                                  fprintf(stderr, "\nEroare nr: 13\n%s\n", mysql_error(conn));
											                            }
											                            else
											                            { 
											                              bzero(&raspuns,sizeof(char)*50);
											                              printf("Contul a fost creat\n");
											                              strcpy(raspuns,"Contul a fost creat");
											                            }
											                            free(interogare);

											                            //trimitem raspunsul clientului, acesta va iesi din starea blocanta 
											                            //despre care vorbeam 

											                      if (write (client, raspuns, sizeof(char)*50) <= 0)
												                    {
												                      perror ("Eroare nr: 14\n");
												                      continue;   
												                    }
											                             else
											                               printf ("[server_register]Mesajul a fost trasmis cu succes.\n");

										pthread_mutex_unlock(&mlock2);		                           	
/* end case 1 */ 				  break; 


						          case 2:
						          		pthread_mutex_lock(&mlock1);
										                        printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== Logare ===\n",client);

										                        		bzero(&rasp_login,sizeof(rasp_login));
										                             if (read (client, &rasp_login, sizeof(struct str_login)) <= 0)
											                            {
											                              perror ("Eroare nr: 15\n");
											                              close (client); 
											                              continue;   
											                            }

										                            printf("\x1b[33m[sock:%d]\x1b[0m Datele primite sunt:\n\tuser: %s\n\tpw:%s\n",client,rasp_login.user,rasp_login.pw);
										                   
										                      /* send SQL query */
										                             char * username=rasp_login.user;
										                             char * comanda ="SELECT * FROM UTILIZATORI where username='";
										                             char * dd;
										                             dd=concatenare(comanda,username);
										                             dd=concatenare(dd,"'"); /* SELECT FROM *
										                                                        WHERE username='stefanuca' */  // cautarea este optimizata cu ajutorul bazei de date

										                           if (mysql_query(conn, dd)) 
										                                fprintf(stderr, "\nEroare nr: 16\n%s\n", mysql_error(conn));
										                      
										                             free(dd); // eliberam rezursele folosite la concatenare

										                            res = mysql_use_result(conn);


										                            int tip_u;
										                          while ((row = mysql_fetch_row(res)) != NULL)
										                           {
										                               if((strcmp(row[0],rasp_login.user)==0)&&(strcmp(row[1],rasp_login.pw)==0))
										                                {
										                                  strcpy(rasp_login.raspuns,"Logat");
										                                  rasp_login.profilul=atoi(row[4]); // preluam din baza de date valoarea profilului cand ne logam
										                                  rasp_login.tip=atoi(row[3]);

										                                  tip_u=atoi(row[3]);
										                                  break;
										                                }
										                           }
										                             mysql_free_result(res);

										                            if(strcmp(rasp_login.raspuns,"Logat")==0)
										                              {
										                                  // vom insera in tabela descriptorul si utilizatorul logat
															                                  //INSERT INTO  `proiectRC`.`utilizatori_online` (`username` ,`sd_client`)VALUES ('username',  '5');
															                  char txt[16];
															                  sprintf(txt, "%d", client); // convertim pt a putea insera in databases sd_client

										                                  char * online_user="INSERT INTO  `proiectRC`.`utilizatori_online` (`username` ,`sd_client`)VALUES ('";
										                                  online_user = concatenare (online_user,rasp_login.user);
										                                  online_user = concatenare(online_user,"',  '");
										                                  online_user = concatenare(online_user, (char*)txt);
										                                  online_user = concatenare(online_user,"')");
										                                  printf("\x1b[33m[sock:%d]\x1b[0mInterodare useri-online: %s\n",client,online_user);
										                                   if (mysql_query(conn, online_user)) 
										                                        fprintf(stderr, "\nEroare nr: 17\n%s\n", mysql_error(conn));
										                                      else
										                                        printf("Am adaugat utilizatorul in baza de date ca fiind logat\n");

										                                  free(online_user);
										                                }


										                             if(tip_u==1) // inseamna ca utilizatorul este administrator
										                             {  // trimitem o notificare catre toti utilizatorii cum ca s-a logat un administrator pe server

										                             	  if (mysql_query(conn, "SELECT * FROM `descriptori_utilizatori_logati_pe_sv`")) 
										                                		fprintf(stderr, "\nEroare nr: 18\n%s\n", mysql_error(conn));

										                                	admin_online_msg=mysql_use_result(conn);

													                          while ((row = mysql_fetch_row(admin_online_msg)) != NULL)
													                           {
													                           		int kkk;

													                           		kkk=atoi(row[0]);

													                           		if(kkk!=client)
													                           		{
													                           			if(send(kkk,"o",1,MSG_OOB)<0){printf("\nEroare nr: 19");}
													                           			if(write(kkk,&rasp_login.user,sizeof(char)*50)<0){printf("\nEroare nr: 20");}
													                           		}
													                           }


										                             }
										                        
										                            printf("\x1b[33m[sock:%d]\x1b[0m este: %s\n",client,rasp_login.raspuns);        



										                          if (write (client,&rasp_login, sizeof(struct str_login)) <= 0)
										                            {
										                              perror ("Eroare nr: 21\n");
										                              continue;   
										                            }
										                          else
										                             printf ("[server]Mesajul a fost trasmis cu succes.\n");

								  pthread_mutex_unlock(&mlock1);
						          break;



						          case 3:
						           		//pthread_mutex_lock(&mlock3);
						          switch(fork())
						          			{
						          				case 0:
										          				printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== Vizualizarea Postarilor Publice ===\n\n\n",client);
										          				//interogare asupra bazei de date
										          				//SELECT * FROM `postari` WHERE `catre_grup`='public'

										          				char * interogare_postari_publice="SELECT * FROM `postari` WHERE `catre_grup`='public'";
										          				char * postarePublicaContinut="\x1b[32m#";
										          				char postarePublicaContinutCatreClient[871];//marimea (50+20+100+500+200caracterele speciale)*sizeof(char)+1

										          				printf("\x1b[33m[sock:%d]\x1b[0mInterogarea supra bazei de date: %s\n",client,interogare_postari_publice );

										          				 if (mysql_query(conn, interogare_postari_publice)) 
										                                        fprintf(stderr, "\nEroare nr: 22\n%s\n", mysql_error(conn));

										                                        postari_publice_pointer=mysql_use_result(conn);

										                          while ((row = mysql_fetch_row(postari_publice_pointer)) != NULL)
										                           {
										                           	   postarePublicaContinut=concatenare(postarePublicaContinut,"\x1b[32m######################\n#\x1b[0m Postat de: ");
										                           	   postarePublicaContinut=concatenare(postarePublicaContinut,row[1]);
										                           	   postarePublicaContinut=concatenare(postarePublicaContinut,"\n\x1b[32m#\x1b[0m Tipul postarii: ");
										                           	   postarePublicaContinut=concatenare(postarePublicaContinut,row[2]);
										                           	   postarePublicaContinut=concatenare(postarePublicaContinut,"\n\x1b[32m#\x1b[0m Titlul postarii: ");
										                           	   postarePublicaContinut=concatenare(postarePublicaContinut,row[3]);
										                           	   postarePublicaContinut=concatenare(postarePublicaContinut,"\n\x1b[32m#\x1b[0m Continutul: ");
										                           	   postarePublicaContinut=concatenare(postarePublicaContinut,row[4]);
										                           	   postarePublicaContinut=concatenare(postarePublicaContinut,"\n\x1b[32m#######################\x1b[0m\n\n");

										                           	   //ne-am pregatit postarea o trimitem clientului
										                           	   
										                           	   strcpy(postarePublicaContinutCatreClient,postarePublicaContinut);
										                           	   printf("%s\n", postarePublicaContinutCatreClient);


										                           	       if (write (client, &postarePublicaContinutCatreClient, sizeof(char)*871) <= 0)
								                                                {
								                                                  perror ("Eroare nr: 23\n");
								                                                  return &errno;
								                                                }  
										                           	   //printf("%s\n", postarePublicaContinut);
										                               //printf("#######################\n# Postat de: %s\n# Tipul postarii: %s\n# Titlul postarii: %s\n# Continutul: %s\n#######################\n\n",row[1],row[2],row[3],row[4]);
										                          		bzero(postarePublicaContinut,sizeof(postarePublicaContinut));
										                          		postarePublicaContinut=concatenare(postarePublicaContinut,"\x1b[32m#");	
										                          		sleep(1);
										                           }

								                           	       if (write (client,"gata", 100) <= 0) //trimitem "gata", ca un flag pt client , sa stiu ca nu mai are nimic de citit
						                                                {
						                                                  perror ("Eroare nr: 24\n");
						                                                  return &errno;
						                                                } 
						            default: 
						                    ;      	  //nimic 
						                    wait(NULL);             
						                                
						         }                
											
										//pthread_mutex_unlock(&mlock3);				                                       
						          break;



						          case 4:
						         		 pthread_mutex_lock(&mlock4);
											                        printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== Setare Profil ===\n",client);                             
											                              if (read (client, &rasp_login, sizeof(struct str_login)) <= 0)
											                            {
											                              perror ("Eroare nr: 25\n");
											                              close (client); 
											                              continue;   
											                            }  
											                            // exemplu interogare;
											                            //UPDATE `proiectRC`.`UTILIZATORI` SET `profil`=1 WHERE `username`='a';
											                            
											                            char raspuns_pt_client_interogare[6];
											                            char txta[16];
											                            sprintf(txta, "%d", rasp_login.profilul); //converteste in (char) pentru concatenare

											                            char * interogaree="UPDATE `proiectRC`.`UTILIZATORI` SET `profil`=";
											                            interogaree=concatenare(interogaree,(char*)txta);
											                            interogaree=concatenare(interogaree," WHERE `username`='");
											                            interogaree=concatenare(interogaree,rasp_login.user);
											                            interogaree=concatenare(interogaree,"';");

											                            printf("\x1b[33m[sock:%d]\x1b[0m Set User Profil: %s\n",client,interogaree);

											                                   if (mysql_query(conn, interogaree)) 
											                                        {fprintf(stderr, "\nEroare nr: 26\n%s\n", mysql_error(conn));
											                                          strcpy(raspuns_pt_client_interogare,"Esec");
											                                        }
											                                      else
											                                        {printf("Am setat cu succes profilul\n");
											                                        strcpy(raspuns_pt_client_interogare,"Succes");
											                                        free(interogaree);
											                                        }
											                                       // printf("%s\n",raspuns_pt_client_interogare );
											              if (write (client, &raspuns_pt_client_interogare, sizeof(char)*6) <= 0)
											                {
											                  perror ("Eroare nr: 27\n");
											                  return &errno;
											                }
											                else
											                  printf("AM trimis cu succes: %s \n" ,raspuns_pt_client_interogare);

										pthread_mutex_unlock(&mlock4);	
								  break;


															          			
						          case 5:
						          		//pthread_mutex_lock(&mlock5);
						          switch(fork())
						          {
						          	case 0:
									                        printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== Adauga un prieten ===\n",client);  

									                        	if (read (client, &rasp_login, sizeof(struct str_login)) <= 0)
									                            {
									                              perror ("Eroare nr: 28\n");
									                              close (client); 
									                              continue;   
									                            }

									                        char * adauga_prieten_list="SELECT username FROM utilizatori_online;";
									                        char * lista_utilizatori_online=" ";
									                        char * lista_prieteni= " ";
									                        char raspuns_catre_client_add_prieten[100];

									                        if (mysql_query(conn, adauga_prieten_list)) 
									                                fprintf(stderr, "\nEroare nr: 29%s\n", mysql_error(conn));
									                            printf("\x1b[33m[sock:%d]\x1b[0m %s\n",client,adauga_prieten_list);

									                            lista_u_online=mysql_use_result(conn);

									                          while ((row = mysql_fetch_row(lista_u_online)) != NULL)
									                           {
									                           	   lista_utilizatori_online=concatenare(lista_utilizatori_online,"\n\t\t\t\t   ");
									                               lista_utilizatori_online=concatenare(lista_utilizatori_online,row[0]);
									                               //printf("%s\n",row[0]);
									                           }
									                           printf("\x1b[33m[sock:%d]\x1b[0mLista utilizatorilor online: %s\n",client,lista_utilizatori_online);
									                           bzero(lista_u_online,sizeof(lista_u_online));
									                           mysql_free_result(lista_u_online);

									                           // lista prietenilor lui stefanuca
									                       		/*SELECT distinct(cod1_uname) FROM `prieteni` WHERE `cod2_uname`='stefanuca' 
																union
																SELECT distinct(cod2_uname) FROM `prieteni` WHERE `cod1_uname`='stefanuca' 
																*/
																char * interogare_prieten="SELECT distinct(cod1_uname) FROM `prieteni` WHERE `cod2_uname`='";
																interogare_prieten=concatenare(interogare_prieten,rasp_login.user);
																interogare_prieten=concatenare(interogare_prieten,"'union SELECT distinct(cod2_uname) FROM `prieteni` WHERE `cod1_uname`='");
																interogare_prieten=concatenare(interogare_prieten,rasp_login.user);
																interogare_prieten=concatenare(interogare_prieten,"';");
																printf("\x1b[33m[sock:%d]\x1b[0mInterogare lista prieteni:\n%s\n",client,interogare_prieten );
			 												
			 												if (mysql_query(conn, interogare_prieten)) 
									                                fprintf(stderr, "\nEroare nr: 30\n%s\n", mysql_error(conn));

																lista_u_prieteni =mysql_use_result(conn);

									                          while ((row = mysql_fetch_row(lista_u_prieteni)) != NULL)
									                           {
									                           	   lista_prieteni=concatenare(lista_prieteni,"\n\t\t");
									                               lista_prieteni=concatenare(lista_prieteni,row[0]);
									                              // printf("%s\n",row[0]);
									                           }
									                           printf("Lista prieteni: %s\n", lista_prieteni);


									                           	//utilizatorii care nu sunt prieteni cu stefanuca
									                           /*
																SELECT username FROM `UTILIZATORI` 
																where `username` not in (
																SELECT distinct(cod1_uname) FROM `prieteni` WHERE `cod2_uname`='stefanuca' 
																union
																SELECT distinct(cod2_uname) FROM `prieteni` WHERE `cod1_uname`='stefanuca' 
																)and `username`<> 'stefanuca'
																*/
																char * utilizatoriiCareNuItiSuntPrieteni="SELECT username FROM `UTILIZATORI` where `username` not in (SELECT distinct(cod1_uname) FROM `prieteni` WHERE `cod2_uname`='";
																utilizatoriiCareNuItiSuntPrieteni=concatenare(utilizatoriiCareNuItiSuntPrieteni,rasp_login.user);
																utilizatoriiCareNuItiSuntPrieteni=concatenare(utilizatoriiCareNuItiSuntPrieteni,"' union SELECT distinct(cod2_uname) FROM `prieteni` WHERE `cod1_uname`='");
																utilizatoriiCareNuItiSuntPrieteni=concatenare(utilizatoriiCareNuItiSuntPrieteni,rasp_login.user);
																utilizatoriiCareNuItiSuntPrieteni=concatenare(utilizatoriiCareNuItiSuntPrieteni,"')");
																utilizatoriiCareNuItiSuntPrieteni=concatenare(utilizatoriiCareNuItiSuntPrieteni,"and `username`<> '");
																utilizatoriiCareNuItiSuntPrieteni=concatenare(utilizatoriiCareNuItiSuntPrieteni,rasp_login.user);
																utilizatoriiCareNuItiSuntPrieteni=concatenare(utilizatoriiCareNuItiSuntPrieteni,"'");
																printf("\x1b[33m[sock:%d]\x1b[0mInterogare lista utilizatori cu care nu esti prieten:\n%s\n",client,utilizatoriiCareNuItiSuntPrieteni );

																if (mysql_query(conn, utilizatoriiCareNuItiSuntPrieteni)) 
								                                fprintf(stderr, "\nEroare nr: 31%s\n", mysql_error(conn));
																lista_u_nu_esti_prieten=mysql_use_result(conn);

																char* lista_u_ca_nu_iti_sunt_prieteni=" ";

									                          while ((row = mysql_fetch_row(lista_u_nu_esti_prieten)) != NULL)
									                           {
									                           	   lista_u_ca_nu_iti_sunt_prieteni=concatenare(lista_u_ca_nu_iti_sunt_prieteni,"\n\t\t                               ");
									                               lista_u_ca_nu_iti_sunt_prieteni=concatenare(lista_u_ca_nu_iti_sunt_prieteni,row[0]);
									                              // printf("%s\n",row[0]);
									                           }
									                           printf("Lista utilizatorilor care nu iti sunt prieteni:%s\n", lista_u_ca_nu_iti_sunt_prieteni);

									                           // utilizatorii online care nu sunt prieteni cu stefanuca
																	/*SELECT username FROM `UTILIZATORI` 
																	where `username` not in (
																	SELECT distinct(cod1_uname) FROM `prieteni` WHERE `cod2_uname`='stefanuca' 
																	union
																	SELECT distinct(cod2_uname) FROM `prieteni` WHERE `cod1_uname`='stefanuca' 
																	) and `username` in (
																	SELECT username 
																	FROM  `utilizatori_online` 
																	)
																	and username <>'stefanuca'	*/
																	char * u_o_care_nu_iti_sunt_prieteni="SELECT username FROM `UTILIZATORI` where `username` not in (SELECT distinct(cod1_uname) FROM `prieteni` WHERE `cod2_uname`='";
																	u_o_care_nu_iti_sunt_prieteni=concatenare(u_o_care_nu_iti_sunt_prieteni,rasp_login.user);
																	u_o_care_nu_iti_sunt_prieteni=concatenare(u_o_care_nu_iti_sunt_prieteni,"' union SELECT distinct(cod2_uname) FROM `prieteni` WHERE `cod1_uname`='")	;
																	u_o_care_nu_iti_sunt_prieteni=concatenare(u_o_care_nu_iti_sunt_prieteni,rasp_login.user);
																	u_o_care_nu_iti_sunt_prieteni=concatenare(u_o_care_nu_iti_sunt_prieteni,"') and `username` in (SELECT username FROM  `utilizatori_online`)and username <>'");
																	u_o_care_nu_iti_sunt_prieteni=concatenare(u_o_care_nu_iti_sunt_prieteni,rasp_login.user);
																	u_o_care_nu_iti_sunt_prieteni=concatenare(u_o_care_nu_iti_sunt_prieteni,"';");
																	printf("\x1b[33m[sock:%d]\x1b[0mInterogare lista utilizatori online care nu iti sunt prieteni (returneaza NULL in caz c anu exista):\n%s\n",client,u_o_care_nu_iti_sunt_prieteni );

																	char * u_o_care_nu_iti_sunt_prieteniLIST=" ";

																		if (mysql_query(conn, u_o_care_nu_iti_sunt_prieteni)) 
									                                fprintf(stderr, "\nEroare nr: 32\n%s\n", mysql_error(conn));
																	lista_uo_nu_esti_prieten=mysql_use_result(conn);

											                          while ((row = mysql_fetch_row(lista_uo_nu_esti_prieten)) != NULL)
											                           {
											                           	   u_o_care_nu_iti_sunt_prieteniLIST=concatenare(u_o_care_nu_iti_sunt_prieteniLIST,"\n\t\t\t                               ");
											                               u_o_care_nu_iti_sunt_prieteniLIST=concatenare(u_o_care_nu_iti_sunt_prieteniLIST,row[0]);
											                              // printf("%s\n",row[0]);
											                           }
											                           printf("Lista utilizatorilor online care nu iti sunt prieteni:%s\n", u_o_care_nu_iti_sunt_prieteniLIST);



									                           	/*
																	am incercat sa trimit catre client variabila de timp (char*) , fara succes
																	nici nu pot declara un verctor char vector[strlen(char*)+1]

																	singura posibilitate este sa trimitem rezultatul printr-un buffer/structura
									                           	*/

																
									                             bzero(&Adauta_un_prieten,sizeof(struct str_adaugaPrieten));
																 strcpy(Adauta_un_prieten.utilizatori_Online,lista_utilizatori_online);
																 strcpy(Adauta_un_prieten.lista_Prieteni,lista_prieteni);
																 strcpy(Adauta_un_prieten.utilizatorii_care_nu_iti_sunt_prieteni,lista_u_ca_nu_iti_sunt_prieteni);
																 strcpy(Adauta_un_prieten.utilizatorii_online_care_nu_iti_sunt_prieteni,u_o_care_nu_iti_sunt_prieteniLIST);

							                                  
							                                  if (write (client, &Adauta_un_prieten, sizeof(struct str_adaugaPrieten)) <= 0)
							                                    {
							                                      perror ("Eroare nr: 33\n");
							                                      return &errno;
							                                    }
							                                    else
							                                    	printf("\x1b[33m[sock:%d]\x1b[0m Am trimis la client\n",client);


							                                    if (read (client, &prieten_sr, sizeof(struct str_prieten)) <= 0)
										                            {
										                              perror ("Eroare nr: 34\n");
										                              close (client); 
										                              continue;   
										                            } 

							                       					printf("\x1b[33m[sock:%d]\x1b[0mAm primit: %s %s %s\n",client,prieten_sr.cod1_uname,prieten_sr.cod2_uname,prieten_sr.tip);

							                       					char * interogare_prieten_add="INSERT INTO `proiectRC`.`prieteni` VALUES (NULL, '";
							                       					// INSERT INTO `proiectRC`.`prieteni` VALUES (NULL, 'stefanuca', 'user1', 'coleg');
							                       					interogare_prieten_add=concatenare(interogare_prieten_add,prieten_sr.cod1_uname);
							                       					interogare_prieten_add=concatenare(interogare_prieten_add,"', '");
							                       					interogare_prieten_add=concatenare(interogare_prieten_add,prieten_sr.cod2_uname);
							                       					interogare_prieten_add=concatenare(interogare_prieten_add,"', '");
							                       					interogare_prieten_add=concatenare(interogare_prieten_add,prieten_sr.tip);
							                       					interogare_prieten_add=concatenare(interogare_prieten_add,"');");

							                       					printf("\x1b[33m[sock:%d]\x1b[0mInterogare add prieten: %s\n",client,interogare_prieten_add);
					                           				
					                           						bzero(raspuns_catre_client_add_prieten,sizeof(raspuns_catre_client_add_prieten));

																if (mysql_query(conn, interogare_prieten_add)) 
																{
									                                fprintf(stderr, "\nEroare nr: 35\n%s\n", mysql_error(conn));
									                                strcpy(raspuns_catre_client_add_prieten,"Incercarea de adaugare a esuat");
									                                }
									                                else
									                                	{
									                                		printf("\x1b[33m[sock:%d]\x1b[0mInterogare realizata cu succes\n",client);
									                                		strcpy(raspuns_catre_client_add_prieten,"Ai adaugat cu succes un nou prieten");
									                           			 }	

										                           	if (write (client, &raspuns_catre_client_add_prieten, sizeof(raspuns_catre_client_add_prieten)) <= 0)
								                                    {
								                                      perror ("Eroare nr: 36\n");
								                                      return &errno;
								                                    }
								                                    else
								                                    	printf("\x1b[33m[sock:%d]\x1b[0mAi trimis raspunsul catre client\n",client );

								                    default:
								                    wait(NULL);                		
								                  }                  		
										//pthread_mutex_unlock(&mlock5);	                                    			
						          break;


						          case 6:
						          		pthread_mutex_lock(&mlock6);
										          		printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== Posteaza o stire ===\n",client);

										          							bzero(&postari_request,sizeof(postari_request));
											                           if (read (client, &postari_request, sizeof(struct str_postari)) <= 0)
											                            {
											                              perror ("Eroare nr: 37\n");
											                              close (client); 
											                              continue;   
											                            }  
											                         printf("Am primit datele urmatoare:\n\tUser: %s\n\tTitlu: %s\n\tContinut postare: %s\n\tCatre grupul: %s\n",postari_request.username,postari_request.titlu,postari_request.continut,postari_request.catre_grupul);

										                         //INSERT INTO `proiectRC`.`postari` VALUES (NULL, 'stefanuca', '(familie/coleg/apropiati)', 'Titlu postare inserare', 'Continut postare inserare');
											                         	char * interogare_postare="INSERT INTO `proiectRC`.`postari` VALUES (NULL, '";
											                         	interogare_postare=concatenare(interogare_postare,postari_request.username);
											                         	interogare_postare=concatenare(interogare_postare,"', '");
											                         	interogare_postare=concatenare(interogare_postare,postari_request.catre_grupul);
											                         	interogare_postare=concatenare(interogare_postare,"', '");
											                         	interogare_postare=concatenare(interogare_postare,postari_request.titlu);
											                         	interogare_postare=concatenare(interogare_postare,"', '");
											                         	interogare_postare=concatenare(interogare_postare,postari_request.continut);
											                         	interogare_postare=concatenare(interogare_postare,"');");
											                         	
											                         	printf("\x1b[33m[sock:%d]\x1b[0mInterogarea: %s\n", client,interogare_postare);
																	
																	char raspuns_catre_client_postare[50];
																	if (mysql_query(conn, interogare_postare)) 
																	{
										                                fprintf(stderr, "\nEroare nr: 38\n%s\n", mysql_error(conn));
										                                strcpy(raspuns_catre_client_postare,"Incercarea de a posta a esuat");
										                                }
										                                else
										                                	{
										                                		printf("\x1b[33m[sock:%d]\x1b[0mInterogare realizata cu succes\n",client);
										                                		strcpy(raspuns_catre_client_postare,"Ai postat cu succes stirea!");
										                           			 }	


									                           			   if (write (client, &raspuns_catre_client_postare, sizeof(char)*50) <= 0)
											                                    {
											                                      perror ("Eroare nr: 39\n");
											                                      return &errno;
											                                    } 

											                                    //facem concatenarea stirii
											                                    char stirePregatire[1000];
											                         printf("\n####################\n# Postat de: %s\n# Titlu: %s\n# Continut postare: %s\n# Catre grupul: %s\n####################",postari_request.username,postari_request.titlu,postari_request.continut,postari_request.catre_grupul);
											                         	fflush(stdout);
											                         	char * stirePregatireConc="\n####################\n# Postat de: ";
											                         	stirePregatireConc=concatenare(stirePregatireConc,postari_request.username);
											                         	stirePregatireConc=concatenare(stirePregatireConc,"\n# Titlu: ");
											                         	stirePregatireConc=concatenare(stirePregatireConc,postari_request.titlu);
											                         	stirePregatireConc=concatenare(stirePregatireConc,"\n# Continut postare: ");
											                         	stirePregatireConc=concatenare(stirePregatireConc,postari_request.continut);
											                         	stirePregatireConc=concatenare(stirePregatireConc,"\n# Catre grupul: ");
											                         	stirePregatireConc=concatenare(stirePregatireConc,postari_request.catre_grupul);
											                         	stirePregatireConc=concatenare(stirePregatireConc,"\n###################");
											                         	stirePregatireConc=concatenare(stirePregatireConc,"#");
											                         	strcpy(stirePregatire,stirePregatireConc);

											                         	printf("%s",stirePregatire);
											                         	fflush(stdout);

											                         	if (strcmp(postari_request.catre_grupul,"public")==0)
											                         	{
											                         		//SELECT * FROM  `descriptori_utilizatori_logati_pe_sv`
											                         		if (mysql_query(conn, "SELECT * FROM  `descriptori_utilizatori_logati_pe_sv`")) 
																					{
														                               fprintf(stderr, "\nEroare nr: 40\n%s\n", mysql_error(conn));
														                            }

														                      Trimit_catre_Public=mysql_use_result(conn);	

																		         while ((row = mysql_fetch_row(Trimit_catre_Public)) != NULL)
															                           {
															                           		int ll;
															                           		ll=atoi(row[0]);

															                           		if(ll!=client)
															                           		{
																		                           		if(send(ll,"p",1,MSG_OOB)<0)
																		                           			{
																		                           				puts("Eroare nr: 41");
																		                           			}

																						                   if (write (ll, &stirePregatire, sizeof(char)*1000) <= 0)
																                                            {
																                                              perror ("Eroare nr: 42\n");
																                                              return &errno;
																                                            } 
																                                            printf("\nAm trimis postarea publica cu succes catre clientul cu descriptorul: %d",ll);
																                                            fflush(stdout);
																                              }
															                           }

															                  mysql_free_result(Trimit_catre_Public);
											                         	}
											                         	else
											                         	{/*
											                         		SELECT * 
																			FROM  `utilizatori_online`
																			where `username` in
																			(
																			SELECT distinct(cod1_uname) FROM `prieteni` WHERE `cod2_uname`='stefanuca' 
																			union
																			SELECT distinct(cod2_uname) FROM `prieteni` WHERE `cod1_uname`='stefanuca' 
																			) */

																			char* int_trm_Post_prieteni_online="SELECT * FROM  `utilizatori_online` where `username` in (SELECT distinct(cod1_uname) FROM `prieteni` WHERE `cod2_uname`='";
																			int_trm_Post_prieteni_online=concatenare(int_trm_Post_prieteni_online,postari_request.username);
																			int_trm_Post_prieteni_online=concatenare(int_trm_Post_prieteni_online,"' union SELECT distinct(cod2_uname) FROM `prieteni` WHERE `cod1_uname`='");
																			int_trm_Post_prieteni_online=concatenare(int_trm_Post_prieteni_online,postari_request.username);
																			int_trm_Post_prieteni_online=concatenare(int_trm_Post_prieteni_online,"')");

																			printf("\nIntegogare: %s",int_trm_Post_prieteni_online);
																			fflush(stdout);
																			if (mysql_query(conn, int_trm_Post_prieteni_online)) 
														                                        fprintf(stderr, "\nEroare nr: 43\n%s\n", mysql_error(conn));

														                     Trimit_catre_DualPublic=mysql_use_result(conn);
												                             while ((row = mysql_fetch_row(Trimit_catre_DualPublic)) != NULL)
													                           {
															                           	int lll;
																	                    lll=atoi(row[1]);
																		                           		if(send(lll,"p",1,MSG_OOB)<0)
																		                           			{
																		                           				puts("Eroare nr: 44");
																		                           			}

																						                   if (write (lll, &stirePregatire, sizeof(char)*1000) <= 0)
																                                            {
																                                              perror ("Eroare nr: 45\n");
																                                              return &errno;
																                                            } 
																                                            printf("\nAm trimis postarea Dualpublica cu succes catre clientul cu descriptorul: %d",lll);
																                                            fflush(stdout);
													                           }

											                         	}



						          		pthread_mutex_unlock(&mlock6);
						          break;
						          


						          case 7:
						          		//	pthread_mutex_lock(&mlock7);
						          			switch(fork())
						          			{
						          				case 0:

														          printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== Vizualizare Postari prieteni ===\n",client);
														          bzero(&rasp_login,sizeof(rasp_login));

														           if (read (client, &rasp_login, sizeof( struct str_login)) <= 0)
									                                            {
									                                              perror ("Eroare nr: 46\n");
									                                              return &errno;
									                                            } 

									                                // stefanuca este utilizatorul care s-a logat catre server si poate vizualiza postarile facute de amicii lui // nu le poate vedea si pe cele facute de el
																/*
																SELECT distinct(`username`), `titlu`, `continut`,`tip`,`catre_grup`
																FROM `postari`p inner join `prieteni` pri on p.`username`=pri.`cod1_uname`or p.`username`=pri.`cod2_uname`
																where (`cod2_uname`='stefanuca' or `cod1_uname`='stefanuca' ) and `tip`=`catre_grup` and `username`<> 'stefanuca'
																*/

														        char postari_PrieteniCategorie[871];//marimea (50+20+100+500+200caracterele speciale)*sizeof(char)+1
																
																char * PostareCategorieContinut="#";
																char * intPostariAmici="SELECT distinct(`username`), `titlu`, `continut`,`tip`,`catre_grup` FROM `postari`p inner join `prieteni` pri on p.`username`=pri.`cod1_uname`or p.`username`=pri.`cod2_uname` where (`cod2_uname`='";
																intPostariAmici=concatenare(intPostariAmici,rasp_login.user);
																intPostariAmici=concatenare(intPostariAmici,"' or `cod1_uname`='");
																intPostariAmici=concatenare(intPostariAmici,rasp_login.user);
																intPostariAmici=concatenare(intPostariAmici,"' ) and `tip`=`catre_grup` and `username`<> '");
																intPostariAmici=concatenare(intPostariAmici,rasp_login.user);
																intPostariAmici=concatenare(intPostariAmici,"';");

																printf("\x1b[33m[sock:%d]\x1b[0mInterogarea: %s\n",client ,intPostariAmici);

																if (mysql_query(conn, intPostariAmici)) 
														                              fprintf(stderr, "\nEroare nr: 47\n%s\n", mysql_error(conn));

														         postariPrieteniPointer=mysql_use_result(conn);	

														         while ((row = mysql_fetch_row(postariPrieteniPointer)) != NULL)
											                           {

											                           		bzero(postari_PrieteniCategorie,sizeof(postari_PrieteniCategorie));
											                           		//printf("#######################\n# Postat de: %s\n# Sunteti prieten de tipul: %s\n# Titlul postarii: %s\n# Continutul: %s\n#######################\n\n",row[0],row[3],row[1],row[2]);

											                           		PostareCategorieContinut=concatenare(PostareCategorieContinut,"######################\n# Postat de: ");
											                           		PostareCategorieContinut=concatenare(PostareCategorieContinut,row[0]);
											                           		PostareCategorieContinut=concatenare(PostareCategorieContinut,"\n# Sunteti prieten de tipul: ");
											                           		PostareCategorieContinut=concatenare(PostareCategorieContinut,row[3]);
											                           		PostareCategorieContinut=concatenare(PostareCategorieContinut,"\n# Titlul postarii: ");
											                           		PostareCategorieContinut=concatenare(PostareCategorieContinut,row[1]);
											                           		PostareCategorieContinut=concatenare(PostareCategorieContinut,"\n# Continutul: ");
											                           		PostareCategorieContinut=concatenare(PostareCategorieContinut,row[2]);
											                           		PostareCategorieContinut=concatenare(PostareCategorieContinut,"\n#######################\n\n");

											                           		
											                           		strcpy(postari_PrieteniCategorie,PostareCategorieContinut);
											                           		printf(">>>Trimitem catre client:\n%s\n\n",PostareCategorieContinut);

											                           		if (write (client, &postari_PrieteniCategorie, sizeof(char)*871) <= 0)
									                                                {
									                                                  perror ("Eroare nr: 48\n");
									                                                  return &errno;
									                                                }  

									                                         bzero(PostareCategorieContinut,sizeof(PostareCategorieContinut));
											                          		PostareCategorieContinut=concatenare(PostareCategorieContinut,"#");
											                          		sleep(1);

											                           }
											                           mysql_free_result(postariPrieteniPointer);

											                           	 if (write (client,"gata", 100) <= 0) //trimitem "gata", ca un flag pt client , sa stiu ca nu mai are nimic de citit
									                                                {
									                                                  perror ("Eroare nr: 49\n");
									                                                  return &errno;
									                                                } 
									                                                else
									                                                	printf("\x1b[33m[sock:%d]\x1b[0mAm terminat de trimis postarile\n",client);
									                                                puts("\n");
									                     default:
									                     	wait(NULL); 	                          
									                  
									                   }                             

										//pthread_mutex_unlock(&mlock7);						                                                	
						          break;


						          case 8:
						          		pthread_mutex_lock(&mlock8);
											          	printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== Chat ===\n",client);

											          			char userNameChat[50];
											          			char userCorectChatConfirmare[50];
											          			bzero(userNameChat,sizeof(char)*50);
											          			bzero(userCorectChatConfirmare,sizeof(char)*50);
											          			bzero(&chat2user_request,sizeof(chat2user_request));

										                           if (read (client, &chat2user_request, sizeof(chat2user_request)) <= 0)
										                            {
										                              perror ("Eroare nr: 50\n");
										                              close (client); 
										                            }	
										                            	//SELECT * FROM `UTILIZATORI` WHERE `username`='stefanuca';
										                            char * interogareChatNumeU1="SELECT * FROM `UTILIZATORI`"; 

										                             if (mysql_query(conn, interogareChatNumeU1)) 
										                                	fprintf(stderr, "\nEroare nr: 51\n%s\n", mysql_error(conn));

										                            numeU1_get=mysql_use_result(conn);

										                             while ((row = mysql_fetch_row(numeU1_get)) != NULL)
											                           {
											                           		if(strcmp(row[0],chat2user_request.user1)==0)
											                           			strcpy(chat2user_request.nume_prenume_u1,row[2]);
											                           		if(strcmp(row[0],chat2user_request.user2)==0)
											                           			strcpy(chat2user_request.nume_prenume_u2,row[2]);
											                           	}
											                           		
											                           												                           /* Verificam daca userul nu este deja in chat*/
											                           //SELECT * FROM  `chat_groups` 
											                             if (mysql_query(conn, "SELECT * FROM  `chat_groups`")) 
									                                        	fprintf(stderr, "\nEroare nr: 52\n%s\n", mysql_error(conn));

					                                        					 chat_verificare_u_group=mysql_use_result(conn);
														                             while ((row = mysql_fetch_row(chat_verificare_u_group)) != NULL)
															                           {
															                           		if(strcmp(row[0],chat2user_request.user2)==0)
															                           		{
															                           			bzero(userCorectChatConfirmare,sizeof(char)*50);
											                           							strcpy(userCorectChatConfirmare,"User chat ocupat");
											                           							printf("\n\n\x1b[33m[sock:%d]\x1b[0m %s",client,userCorectChatConfirmare);
											                           							 goto trimite_rasp_cerere_chat;
											                           							break;
															                           		}

															                           	}


										                            char * interogareCHat = "SELECT * FROM `utilizatori_online`";
										                            puts(interogareCHat);

										                            if (mysql_query(conn, interogareCHat)) 
										                                	fprintf(stderr, "\nEroare nr: 53\n%s\n", mysql_error(conn));
																	
										                            postChatVerificare=mysql_use_result(conn);
						  
										                            strcpy(userCorectChatConfirmare,"User gresit");
										                            int sd_chat_user_confirm;
										                            bool se_poate_insera=false;
										                            char * interogare_Chat_group1="INSERT INTO `proiectRC`.`chat_groups` VALUES ('";
													                char txtas[16];
													                char * interogare_Chat_group2="INSERT INTO `proiectRC`.`chat_groups` VALUES ('";

											                          while ((row = mysql_fetch_row(postChatVerificare)) != NULL)
											                           {
											                           		if(strcmp(chat2user_request.user2,row[0])==0)
											                           		{
											                           			bzero(userCorectChatConfirmare,sizeof(char)*50);
											                           			strcpy(userCorectChatConfirmare,"User valid");

											                           			/* trimitem cererea de chat catre user 2; acesta va trebui sa confirme*/
											                           			
											                           			sd_chat_user_confirm = atoi (row[1]);

											                           			if(send(sd_chat_user_confirm,"c",1,MSG_OOB)<0)
											                           			{
											                           				puts("Eroare nr: 54");
											                           			}

											                           			/*trimitem structura pt ca utilizatorul sa stie de la cine este cererea */
													                           if (write (sd_chat_user_confirm, &chat2user_request, sizeof(chat2user_request)) <= 0)
													                            {
													                              perror ("Eroare nr: 55\n");
													                            }


													                            //INSERT INTO `chat_groups` VALUES ('stefanuca',5,NULL)  // username,descriptor_socket,null
													                            interogare_Chat_group1=concatenare(interogare_Chat_group1,chat2user_request.user1);
													                            interogare_Chat_group1=concatenare(interogare_Chat_group1,"',");
													                           
										                					    sprintf(txtas, "%d", client); // descriptorul clientului il converim pt a face concatenarea
										                					    interogare_Chat_group1=concatenare(interogare_Chat_group1,txtas);
										                					    interogare_Chat_group1=concatenare(interogare_Chat_group1,",NULL)");

										                					    
													                            interogare_Chat_group2=concatenare(interogare_Chat_group2,chat2user_request.user2);
													                            interogare_Chat_group2=concatenare(interogare_Chat_group2,"',");
									
										                					    interogare_Chat_group2=concatenare(interogare_Chat_group2,row[1]);
										                					    interogare_Chat_group2=concatenare(interogare_Chat_group2,",NULL)");

													                            se_poate_insera=true;
													                            break;
														                 }              		
											                           }

											                           mysql_free_result(postChatVerificare);


											                           if(se_poate_insera)
											                           {


														                            if (mysql_query(conn, interogare_Chat_group1)) 
														                                	fprintf(stderr, "\nEroare nr: 56\n%s\n", mysql_error(conn));

														                            if (mysql_query(conn, interogare_Chat_group2)) 
														                                	fprintf(stderr, "\nEroare nr: 57\n%s\n", mysql_error(conn));


														                                printf("Interogari baza de date:\n%s\n%s",interogare_Chat_group1,interogare_Chat_group2);

											                           }

											                           printf("\n\n\x1b[33m[sock:%d]\x1b[0m %s\n",client,userCorectChatConfirmare);
											                           fflush(stdout);

trimite_rasp_cerere_chat:
											                          mysql_free_result(chat_verificare_u_group);
											                           //trimiterea raspunsului
									                                  if (write (client, &userCorectChatConfirmare, sizeof(char)*50) <= 0)
									                                    {
									                                      perror ("Eroare nr: 58\n");
									                                      return &errno;
									                                    }
							

									                          
										pthread_mutex_unlock(&mlock8);					                          
						          break;


						          case 9:
						          		 pthread_mutex_lock(&mlock9);
									                        printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== Delogare ===\n",client);  
										                           if (read (client, &rasp_login, sizeof(struct str_login)) <= 0)
										                            {
										                              perror ("Eroare nr: 59\n");
										                              close (client); 
										                              continue;   
										                            }  
										                            //printf("%s %s\n",rasp_login.raspuns ,rasp_login.user);
										                            //DELETE FROM `proiectRC`.`utilizatori_online` WHERE `username`='a';
										                            char * interg_delogare="DELETE FROM `proiectRC`.`utilizatori_online` WHERE `username`='";
										                            interg_delogare=concatenare(interg_delogare,rasp_login.user);
										                            interg_delogare=concatenare(interg_delogare,"';");
									                          	  
									                          	    printf("\x1b[33m[sock:%d]\x1b[0m %s\n",client,interg_delogare);
									                                   if (mysql_query(conn, interg_delogare)) 
									                                        {fprintf(stderr, "\nEroare nr: 60\n%s\n", mysql_error(conn));
									                                        }
									                                      else
									                                      {
									                                      		printf("\x1b[33m[sock:%d]\x1b[0m s-a delogat\n",client);
									                                      		strcpy(rasp_login.raspuns,"Nelogat");
									                                      		free(interg_delogare);
									                                      }
									                                  if (write (client, &rasp_login, sizeof(struct str_login)) <= 0)
									                                    {
									                                      perror ("Eroare nr: 61\n");
									                                      return &errno;
									                                    } 
									                                     printf("\x1b[33m[sock:%d]\x1b[0m Am trimis noile date catre client\n ",client);

										pthread_mutex_unlock(&mlock9);                                     		
						          break; //end delogare


						          case 10:
						          		 pthread_mutex_lock(&mlock10);
								          			//printf("\nChat mesaje %d \n",client);
								          				bzero(&msg_chat_request,sizeof(msg_chat_request));
								          				if(read(client,&msg_chat_request,sizeof(msg_chat_request))<0)
								          				{
								          					printf("\nEroare nr: 62");
								          				}
 
											                  if((strcmp(msg_chat_request.msg,"exit")==0)||(strcmp(msg_chat_request.msg,"quit")==0))
											                      {
											                      	// stergem din baza de date, de la chat
											                      	//DELETE FROM `chat_groups` WHERE `username` = '';
											                      	char * int_dele_chat_gr="DELETE FROM `chat_groups` WHERE `username` = '";
											                      	int_dele_chat_gr=concatenare(int_dele_chat_gr,msg_chat_request.userr);
											                      	int_dele_chat_gr=concatenare(int_dele_chat_gr,"'");
											                      	puts(int_dele_chat_gr);


											                      	 if (mysql_query(conn, int_dele_chat_gr)) 
									                                        fprintf(stderr, "\nEroare nr: 63\n%s\n", mysql_error(conn));

									                                                  	char * concat_culoare2="Utilizatorul \x1b[31m";
																	              		concat_culoare2=concatenare(concat_culoare2,msg_chat_request.userr);
																	              		concat_culoare2=concatenare(concat_culoare2,"\x1b[0m a iesit din chat");
																	              		strcpy(msg_chat_request.msg,concat_culoare2);


									                                    printf("L-am sters");
									                                    fflush(stdout);

																          					 if (mysql_query(conn, "SELECT * FROM  `chat_groups`")) 
																	                                        fprintf(stderr, "\nEroare nr: 64\n%s\n", mysql_error(conn));

																	                                   // user: mesaj;
																	                                    char* conc_msg_rec;
																	                                    conc_msg_rec=concatenare("\x1b[35m",msg_chat_request.userr);
																	                                    conc_msg_rec=concatenare(conc_msg_rec,"\x1b[0m: ");
																	                                    conc_msg_rec=concatenare(conc_msg_rec,msg_chat_request.msg);

																	                                    strcpy(msg_chat_request.msg,conc_msg_rec);
																	                                    printf("\n%s\n",msg_chat_request.msg);


																		                       get_Desc_chat_grup=mysql_use_result(conn);
																		                             while ((row = mysql_fetch_row(get_Desc_chat_grup)) != NULL)
																			                           {
																			                           		int Desc_chat;
																			                           		Desc_chat=atoi(row[1]);

																			          						if(Desc_chat!=client)
																				          					if(write(Desc_chat,&msg_chat_request,sizeof(msg_chat_request))<0)
																				          					{
																				          						printf("\nEroare nr: 65\n");
																				          					}

																			                           	}

											                      }
											                      else
															            if(strcmp("\n",msg_chat_request.msg)!=0)
												          				{ 
												          					//SELECT * FROM  `chat_groups`;

												          					 if (mysql_query(conn, "SELECT distinct(`username`), `descriptor`, `nume_grup` FROM `chat_groups`")) 
													                                        fprintf(stderr, "\nEroare nr: 66\n%s\n", mysql_error(conn));

													                                   // user: mesaj;
													                                    char* conc_msg_rec;
													                                    conc_msg_rec=concatenare("\x1b[35m",msg_chat_request.userr);
													                                    conc_msg_rec=concatenare(conc_msg_rec,"\x1b[0m: ");
													                                    conc_msg_rec=concatenare(conc_msg_rec,msg_chat_request.msg);

													                                    strcpy(msg_chat_request.msg,conc_msg_rec);
													                                    printf("\n%s\n",msg_chat_request.msg);


														                       get_Desc_chat_grup=mysql_use_result(conn);
														                             while ((row = mysql_fetch_row(get_Desc_chat_grup)) != NULL)
															                           {
															                           		int Desc_chat;
															                           		Desc_chat=atoi(row[1]);

															          						if(Desc_chat!=client)
																          					if(write(Desc_chat,&msg_chat_request,sizeof(msg_chat_request))<0)
																          					{
																          						printf("\nEroare nr: 67");
																          					}

															                           	}

												          				}
												          				

								         pthread_mutex_unlock(&mlock10); 				
						          break;

										          case 11:
										         			 printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== Adauga prieten pentru conversatie [CHAT] ===\n",client); 
										         			 char getUserName[50];
										         			 char raspuns_getUserName[50];

						                                
						                                if (read (client, &chat2user_request, sizeof(chat2user_request)) <= 0)
						                                  {
						                                    perror ("Eroare nr: 68\n");
						                                  }


										         			 bzero(&getUserName,sizeof(getUserName));
													     if(read(client,&getUserName,sizeof(getUserName))<0)
											              {
											              	printf("\nEroare nr: 69");
											              }
											            

											              //SELECT * FROM  `utilizatori_online` 
											              if (mysql_query(conn, "SELECT * FROM  `utilizatori_online`")) 
									                                fprintf(stderr, "\nEroare nr: 70 \n%s\n", mysql_error(conn));
									                            lista_u_online2=mysql_use_result(conn);

									                            strcpy(raspuns_getUserName,"gresit");
									                           bool run_sql_comm=false;
									                           char * interog_chat="INSERT INTO `proiectRC`.`chat_groups` VALUES ('";
									                          while ((row = mysql_fetch_row(lista_u_online2)) != NULL)
									                           {
										                         	if (strcmp(row[0],getUserName)==0)
										                         	{
										                         		run_sql_comm=true;
										                         		bzero(&raspuns_getUserName,sizeof(raspuns_getUserName));
										                         		strcpy(raspuns_getUserName,"corect");

										                         		//inseram in baza de date in chat group
										                         			interog_chat=concatenare(interog_chat,row[0]);
										                         			interog_chat=concatenare(interog_chat,"',");
										                         			interog_chat=concatenare(interog_chat,row[1]);
										                         			interog_chat=concatenare(interog_chat,",NULL)");
										                         			puts(interog_chat);

										                         		//trimitem cerere de chat
										                         		int desc_user;
										                         		desc_user=atoi(row[1]);

									                           			if(send(desc_user,"c",1,MSG_OOB)<0)
									                           			{
									                           				puts("Eroare nr: 71");
									                           			}

										                                if (write (desc_user, &chat2user_request, sizeof(chat2user_request)) <= 0)
										                                  {
										                                    perror ("Eroare nr: 72");
										                                  }
										                                  break;
										                         	}
										                       }		                                  

										                  mysql_free_result(lista_u_online2);

										                  if(run_sql_comm)
										                  {
					                         				if (mysql_query(conn, interog_chat)) 
				                                					fprintf(stderr, "\nEroare nr: 73\n %s\n", mysql_error(conn));
										                  }


										                  printf("Raspunsul este %s",raspuns_getUserName);
										                  fflush(stdout);
														if (write (client, raspuns_getUserName, sizeof(raspuns_getUserName)) <= 0)
						                                  {
						                                    perror ("Eroare nr: 74\n");
						                                  }

						            break;     


                                  case 12:
                                 		 printf("\n\n\x1b[33m[sock:%d]\x1b[0m=== ADMIN::Trimite mesaj catre toti  ===\n",client); 
                                         	 bzero(&admin_request,sizeof(struct str_adminMsg));
                                         	  if (read (client, &admin_request, sizeof(struct str_adminMsg)) <= 0)
		                                          {
		                                            perror ("Eroare nr: 75\n");
		                                          }

		                                          //printf("\nADMIN: %s\nMSG: %s",admin_request.user,admin_request.msg);
		                                          //fflush(stdout);

		                                          //com trimite mesajul catre toti utilizatorii de pe servar, inclusiv cei nelogati
		                                          //SELECT * FROM  `descriptori_utilizatori_logati_pe_sv`

		                                          if (mysql_query(conn, "SELECT * FROM  `descriptori_utilizatori_logati_pe_sv`")) 
					                               		 fprintf(stderr, "\nEroare nr: 76\n%s\n", mysql_error(conn));

					                               		admin_send_msg=mysql_use_result(conn);
					                               while ((row = mysql_fetch_row(admin_send_msg)) != NULL)
							                           {
							                           		int des;
							                           		des=atoi(row[0]);
							                           		if(des!=client)  // trimitem mesajul la clienti
							                           		{
							                           			;
							                           			if(send(des,"t",1,MSG_OOB)<0) {printf("\nEroare nr: 77\n");}
							                           			if (write (des, &admin_request, sizeof(struct str_adminMsg)) <= 0){perror ("Eroare nr: 78\n");}
							                           			printf("\nAm trimis mesajul catre utilizatorul cu descriptorul %d\n",des);
							                           			fflush(stdout);
							                           		}
							                           }

                                  break;
						      } // end switch

						} // while2

}


//Cum am zis si in surs a 'c.c', aceasta metoda merge pe serverele concurente de tip "fork()", acolo nu aveam probleme cand citeam recv(client,&buff, sizeof(buff), MSG_OOB)<0), fiindca client era global;
	// datorita faptului ca nu pot citi 

/*
void semnale(int s)
{
	char buff;
	if(recv(client,&buff, sizeof(buff), MSG_OOB)<0)
			printf("Eroare citire semnal\n");
		printf("\n%d ",s);

	printf("\nSemnale\n");
	printf("Buffer %c",buff);
       
       
        switch(buff)
        {
        	case 'z':
        			printf("\n\n\x1b[33m[sock:%d]\x1b[0m === Utilizatori online ===\n",(int)client);

					           		bzero(&rasp_login,sizeof(rasp_login));
							           if (read (client, &rasp_login, sizeof( struct str_login)) <= 0)
		                                            {
		                                              perror ("Eroare nr: 13\n");
		                                            } 

					                        char * util_online="SELECT username FROM utilizatori_online;";
					                        char * lista_util_online="";
					                        printf("\n\n\x1b[33m[sock:%d]\x1b[0m Interogare: %s",(int)client,util_online);

					                        if (mysql_query(conn, util_online)) 
					                                fprintf(stderr, "%s\n", mysql_error(conn));
												mysql_free_result(lista_u_online);
					                            lista_u_online=mysql_use_result(conn);
					                            char  user_online[67];

					                            
					                            send(client,"2",1,MSG_OOB);
					                            
					                            printf("utilizatorii online sunt: ");
					                          while ((row = mysql_fetch_row(lista_u_online)) != NULL)
					                           {
						                           	bzero(user_online,sizeof(user_online));
						                           	strcpy(user_online,row[0])	;

					                           	   //lista_util_online=concatenare(lista_util_online,"\x1b[35m ");
					                               //lista_util_online=concatenare(lista_util_online,row[0]);
					                               //lista_util_online=concatenare(lista_util_online,"\x1b[0m ");

					                               printf("\x1b[35m\n%s\x1b[0m",row[0]);

							                           if (write (client,&user_online , sizeof(char)*67) <= 0)
		                                                {
		                                                  perror ("Eroare nr: 77\n");
		                                                } 
		                                                sleep(1);
					                           }
					                           puts("                                                                  .");

					                          // printf("\x1b[33m[sock:%d]\x1b[0mLista utilizatorilor online:\n%s\n\n",client,lista_util_online);
                                              if (write (client, "gata", sizeof(char)*67) <= 0)
                                                {
                                                  perror ("Eroare nr: 77\n");
                                                } 


        	break;
        }
	bzero(&buff,sizeof(buff));
}

*/