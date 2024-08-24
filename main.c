//fare relazione
//variabili di tipo in out o lavoro definendo il range di valori ammissibili ed spiegandone l'utilizzo
//spiegazione sull'uso del programma quindi delle funzioni e procedure
//descrizione vincoli programma e statistica efficacia
//Spiegare che funziona al 100 % eliminando attaccoForzato

//  main.c
//  battagliaNavale
//  Created by Vittorio Piotti on 18/02/2022.
//

/*
 * Algoritmo Dijkstra C v1.0.0 (https://github.com/vittorioPiotti/Battaglia-Navale-C/releases/tag/1.0.0)
 * Copyright 2024 Vittorio Piotti
 * Licensed under GPL-3.0 (https://github.com/vittorioPiotti/Battaglia-Navale-C/blob/main/LICENSE.md)
*/




#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#define K 10                                            //dimensione massima del nome del giocatore
#define X 10                                            //colonne - asse x
#define Y 10                                            //righe - asse y
#define P 3                                             //dim sottomarino
#define O 2                                             //dim torpediniere/corvetta
#define U 1                                             //dim lancia
#define M 10                                            //numero di spazi totale occupato dalle navi - ESSENZIALE DA SPECIFICARE CORRETTAMENTE
#define L 1                                             //considera solo gli 1 per la conta degli spazi occupati dalle navi per verificarne la corretta disposizione casuale
#define V 4                                             //considera solo i 4 per controllare chi per primo vince affondando tutte le navi nemiche
//Gestione campi
void azzeraCampo ( int [Y][X] );                           //assegna ad ogni elemento della matrice 0
void visualeRelativa( int, int, int [Y][X] );                //stampa la matrice visualizzando solo gli elementi scoperti
void visualeAssoluta ( int, int, int [Y][X] );                 //stampa la matrice visualizzando tutti gli elementi
void updateCampo ( int [Y][X], int [Y][X], int [Y][X] );     //aggiorna la visualizzazione a schermo dei campi
void copiaCampo ( int [Y][X], int [Y][X] );                //copia gli elementi di una matrice in un altra per far si che lo stesso campo sia gestito in modi diversi
//Gestione schieramento navi

int genCoordManX ( char [] );                              //inserimento in input del valore della coordinata x
int genCoordManY ( char [] );                              //inserimento in input del valore della coordinata y

///manuale - l'utente sceglie in input la direzione verso cui schierare la nave: 1.alto;2.destra;3.basso;4.sinistra
int controlloSchieraTop ( int, int, int, int, int [Y][X] ); //se in base alla dimensione e alla posizione della nave è possibile schierarla top allora fallo altrimenti richiedi l'inserimento delle coordinate
int controlloSchieraDex ( int, int, int, int, int [Y][X] );
int controlloSchieraBot ( int, int, int, int, int [Y][X] );
int controlloSchieraSin ( int, int, int, int, int [Y][X] );
void orientaNaveUpMan ( char [], int, int [Y][X] ); //in base alle coordinate inserite dall'utente schiera
void orientaNaveDexMan ( char [], int, int [Y][X] );
void orientaNaveBotMan ( char [], int, int [Y][X] );
void orientaNaveSinMan ( char [], int, int [Y][X] );
///automatica - attraverso la funzione orientaNave in base al valore assunto dalla variabile di ritorno schiera la nave nella direzione associata: 1.alto;2.destra;3.basso;4.sinistra
void orientaNaveUp ( int, int [Y][X] );
void orientaNaveDex ( int, int [Y][X] );
void orientaNaveBot ( int, int [Y][X] );
void orientaNaveSin ( int, int [Y][X] );

int controllo ( int,int [Y][X] );                        //1-verifica se lo spazio occupato dalle navi è pari a quello che dovrebbero occupare - implicazione nel cotnrollo della vittoria
//2-verifica se la partita è vinta o persa in base al numero di navi colpite in un campo
void generaNave ( int, int [Y][X] );                       //Posiziona le navi nel campo a seconda delle loro coordinate e della loro direzione di schieramento


//Gestione attacchi
int controlloAttacco ( int, int , int [Y][X] );            //Controllo affinche uno stesso punto gia colpito non venga colpito nuovamente
void EsitoAttacco ( int, int, char [], int [Y][X] );          //Eseguito il controllo dell'attacco stampa a schermo se ha colpito l'acqua o una nave
void attacco ( char [], int [Y][X] );                       //L'utente esegue l'attacco manualmente
void autoAttacco ( char [], int [Y][X] );                   //L'attacco è eseguito atuomaticamente
void affondaNave ( int [Y][X] );                           //controlla se una nave è stata affondata ovvero se ogni punto della nave è stato colpito e se ciò accade la casella rappresentativa della nave assume valore 4


//implementazione dei ragionamenti logici  del gioco della battaglia navale applicati all'algoritmo
int attaccoForzato ( char [], int [Y][X] );
float precisione ( int, int);

//stati di una casella:
///0-punto acqua
///1-punto nave
///2-acqua colpito
///3-punto nave colpito
///4 tutti i punti della nave colpiti


//
int main(int argc, const char * argv[]) {
    //dichiaro il generatore di numeri casuale
    srand ( time ( NULL ) );
    //dichiaro il vetore di char con le coordinate alfabetiche delle mappe di gioco
    char lett[X];
    //assegnazione dei valori del vettore di chat in base alle coordinate alfabetiche della mappa di gioco
    lett[0] = 'A';
    lett[1] = 'B';
    lett[2] = 'C';
    lett[3] = 'D';
    lett[4] = 'E';
    lett[5] = 'F';
    lett[6] = 'G';
    lett[7] = 'H';
    lett[8] = 'I';
    lett[9] = 'J';
    //dichiarazione delle variabili di scelta
    int choice;
    int moneta; //variabile specifica per il testa o croce
    int scelta;
    //dichiarazione dei campi di gioco
    int campoIA[Y][X]; //campo di gioco del bot nemico che verra aggiornato e quindi modificato ad ogni attacco
    int campoIA2[Y][X]; //campo di gioco del bot nemico che verra copiato in questa matrice per essere salvato nello stato inziale al fine di stamparlo alla fine mostrando la disposizione originale delle navi
    int myCampo[Y][X]; //stesso ragionamento del campo originale
    int myCampo2[Y][X]; //stesso ragionamento del campo copiato
  //dichiarazione delle variabili contatori
    int cNave1; //conta le navi da 3 caselle schierate (se uguale a uno schierate tutte)
    int cNave2; //conta le anvi da 2 schierate (se uguale a due schierate tutte)
    int cNave3; //conta le navi da 3 schierate (se uguale a 3 schierate tutte)
    //dichiarazione della variabili contatori specifiche per i cicli for
    int i;
    int j;
    //dichiarazione della variabile che salva l'indice del turno in modo tale da essere
    int saveI = 0;
    char nick[K];
    cNave1 = 0;
    cNave2 = 0;
    cNave3 = 0;




    do {
        azzeraCampo(campoIA);
        generaNave(P,campoIA);
        generaNave(O,campoIA);
        generaNave(O,campoIA);
        generaNave(U,campoIA);
        generaNave(U,campoIA);
        generaNave(U,campoIA);
    } while ( controllo(L,campoIA) != 0 );
    copiaCampo(campoIA,campoIA2);
    azzeraCampo(myCampo);
    copiaCampo(myCampo,myCampo2);
    printf ( "\n\n\tQuesta è battaglia navale!\n\n\tIl tuo scopo è quello di affondare navi.\n\n\tLe flotte sono composte da:\n\t\ta. 1 sottomarino (3 caselle)\n\t\tb. 2 torpedinieri (2 caselle)\n\t\tc. 3 lancie (1 casella)\n\n\tIl gioco è a turni\n\n\tVince chi affonda prima le navi nemiche\n\n\tLegenda:\n\t\t? = non colpito\n\t\t- = acqua\n\t\tO = nave colpita\n\t\tX = nave abbattuta\n" );
    do {
        printf("\n\tInserisci il tuo nickname: ");
        scanf( "%s", nick );
    } while ( strlen ( nick ) > K  );


            printf ( "\n\nA sinistra il tuo campo\t\t\t\tAl centro la visuale nemica sul tuo\t\t\tA destra quello nemico\n" );
    updateCampo(myCampo2,myCampo,campoIA);
    printf ( "\n\n\tSchiera navi\n\t\t1.automaticamente\n\t\t2.manualmente\n\n" );
    do {
        printf ( "\tscelgo: " );
        scanf( "%d", &choice );
    } while ( choice != 1 && choice != 2 );
    switch( choice ) {
        case 1:
            do {
                azzeraCampo(myCampo);
                generaNave(P,myCampo);
                generaNave(O,myCampo);
                generaNave(O,myCampo);
                generaNave(U,myCampo);
                generaNave(U,myCampo);
                generaNave(U,myCampo);
            } while ( controllo(L,myCampo) != 0 );

            break;
        case 2:
            azzeraCampo(myCampo);
            do {
                printf ( "\n\n\tScegli orientamento nave:\n\t\t1.alto\n\t\t2.destra\n\t\t3.basso\n\t\t4.sinistra\n\n" );
                do {
                    printf ( "\tScelgo: " );
                    scanf ( "%d", &scelta );
                }while ( scelta < 1 || scelta > 4 );

                switch( scelta ){
                    case 1:
                        printf ( "\n\tscegli nave da schierare:\n\t\t1.sottomarino\n\t\t2.torpediniere\n\t\t3.lancia\n\n" );
                        do {
                            printf ( "\tScelgo: " );
                            scanf ( " %d", &choice );
                        }while ( choice < 1 || choice > 3 );
                        if ( choice == 1 && cNave1 == 0 ) {
                            orientaNaveUpMan(lett,P,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave1++;

                        }
                        else if ( choice == 2 && cNave2 < 2 ) {
                            orientaNaveUpMan(lett,O,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave2++;
                        }
                        else if ( choice == 3 && cNave3 < 3 ) {
                            orientaNaveUpMan(lett,U,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave3++;
                        }

                        break;
                    case 2:
                        printf ( "\n\tscegli nave da schierare:\n\t\t1.sottomarino\n\t\t2.torpediniere\n\t\t3.lancia\n\n" );
                        do {
                            printf ( "\tScelgo: " );
                            scanf ( " %d", &choice );
                        }while ( choice < 1 || choice > 3 );
                        if ( choice == 1 && cNave1 == 0 ) {
                            orientaNaveDexMan(lett,P,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave1++;
                        }
                        else if (choice == 2 && cNave2 < 2 ) {
                            orientaNaveDexMan(lett,O,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave2++;
                        }
                        else if ( choice == 3 && cNave3 < 3 ) {
                            orientaNaveDexMan(lett,U,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave3++;
                        }


                        break;
                    case 3:
                        printf ( "\n\tscegli nave da schierare:\n\t\t1.sottomarino\n\t\t2.torpediniere\n\t\t3.lancia\n\n" );
                        do {
                            printf ( "\tScelgo: " );
                            scanf ( " %d",&choice );
                        } while ( choice < 1 || choice > 3 );
                        if ( choice == 1 && cNave1 == 0){
                            orientaNaveBotMan(lett,P,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave1++;
                        }
                        else if ( choice == 2 && cNave2 < 2 ) {
                            orientaNaveBotMan(lett,O,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave2++;
                        }
                        else if ( choice == 3 && cNave3 < 3 ) {
                            orientaNaveBotMan(lett,U,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave3++;
                        }
                        break;
                    case 4:
                        printf ( "\n\tscegli nave da schierare:\n\t\t1.sottomarino\n\t\t2.torpediniere\n\t\t3.lancia\n\n" );
                        do{
                            printf ( "\tScelgo: " );
                            scanf (" %d", &choice );
                        }while ( choice < 1 || choice > 3 );
                        if ( choice == 1 && cNave1 == 0){
                            orientaNaveSinMan(lett,P,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave1++;
                        }
                        else if ( choice == 2 && cNave2 < 2 ){
                            orientaNaveSinMan(lett,O,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave2++;
                        }
                        else if( choice == 3 && cNave3 < 3 ) {
                            orientaNaveSinMan(lett,U,myCampo);
                            copiaCampo(myCampo,myCampo2);
                            updateCampo(myCampo2,myCampo,campoIA);
                            cNave3++;
                        }
                }


                if ( cNave1 == 0 ) printf ( "\n\nHai un sottomarino da schierare!" );
                else printf ( "\n\nNon hai altri sottomarini da schierare!" );
                if ( cNave2 == 0 ) printf ( "\n\nHai altri due torpedinieri da schierare!" );
                else if ( cNave2 == 1 ) printf ( "\n\nHai un'altro torpediniere da schierare!" );
                else printf ( "\n\nNon hai altri torpedinieri da schierare!" );
                if ( cNave3 == 0 ) printf ( "\n\nHai altre tre lancie da schierare!" );
                else if ( cNave3 == 1 ) printf ( "\n\nHai altre due lancie da schierare!" );
                else if ( cNave3 == 2 ) printf ( "\n\nHai un altra lancia da schierare! ");
                else printf ( "\n\nNon hai altre lancie da schierare!" );

            }while ( cNave1 != 1 || cNave2 != 2 || cNave3 != 3 );
    }
    copiaCampo(myCampo,myCampo2);
    updateCampo(myCampo2,myCampo,campoIA);
    printf ( "\n\nLe navi sono state schierate" );


    printf ( "\n\n\tTesta o croce per iniziare...\n\t\t1.testa\n\t\t2.croce\n\n" );
    do {
        printf ( "\tscelgo: " );
        scanf ( "%d", &scelta );
    } while ( scelta != 1 && scelta != 2 );
    moneta = 1 + rand() % 2;
    if ( moneta == 1 ) printf ( "\n\tEd è testa!\n" );
    else printf ( "\n\tEd è croce!\n" );
    printf ( "\nDiamo inizio alla partita!\n\n" );
    printf ( "\tScegli se:\n\t\t1.simulare la partita\n\t\t2.Giocare la partita\n\n\tscelgo: " );
    do{
        scanf ( "%d", &choice);
    }while ( choice!= 1 && choice != 2);

            if ( scelta != moneta ){
                printf ( "\n\n\n\n\n\n\n\n\n\n\n\n\tTURNO: 1" );
                printf ( "\n\nGuarda e impara!" );
                autoAttacco(lett,myCampo);
                affondaNave(myCampo);
                updateCampo(myCampo2,myCampo,campoIA);

                printf ( "\n\nLa mossa spetta a %s",nick );
                attacco(lett,campoIA);
                affondaNave(campoIA);
                updateCampo(myCampo2,myCampo,campoIA);

                for ( i = 1; i < 99; i ++ ) {
                    printf ( "\n\n\n\n\n\n\n\n\n\n\n\n\tTURNO: %2d\n", i + 1 );
                    printf ( "\n\nLa mossa spetta al nemico" );
                    autoAttacco(lett,myCampo);
                    affondaNave(myCampo);
                    updateCampo(myCampo2,myCampo,campoIA);
                    if ( controllo(V,myCampo) == 0 ) {
                        printf ( "\n\n\n\n\nHai perso!\n\n" );
                        printf ( "\n\tLa precisione del nemico è stata del %3.2f%%\n\n", nick,  precisione ( M, i ) );
                        break;
                    }
                    printf ( "\n\nLa mossa spetta a %s",nick );

                    if ( choice == 1)autoAttacco(lett,campoIA);
                    else attacco(lett,campoIA);
                    affondaNave(campoIA);
                    updateCampo(myCampo2,myCampo,campoIA);
                    if ( controllo(V,campoIA) == 0 ){
                        printf ( "\n\n\n\n\nHAI VINTO!\n\n" );
                        printf ( "\n\tLa precisione di %s è stata del %3.2f%%\n\n", nick,  precisione ( M, i ) );
                        break;
                    }
                }
            }
            else {
                printf ( "\n\n\n\n\n\n\n\n\n\n\n\n\tTURNO: 1" );
                printf ( "\n\nLa mossa spetta a %s",nick );
                updateCampo(myCampo2,myCampo,campoIA);
                attacco(lett,campoIA);
                affondaNave(campoIA);
                updateCampo(myCampo2,myCampo,campoIA);
                printf ( "\n\nLa mossa spetta al nemico" );
                autoAttacco(lett,myCampo);
                affondaNave(myCampo);
                updateCampo(myCampo2,myCampo,campoIA);
                for(i = 1; i < 99; i++ ){
                    printf( "\n\n\n\n\n\n\n\n\n\n\n\n\tTURNO: %2d\n" , i + 1 );
                    printf ( "\n\nLa mossa spetta a %s",nick );

                    if ( choice == 1)autoAttacco(lett,campoIA);
                    else attacco(lett,campoIA);
                    affondaNave(campoIA);
                    updateCampo(myCampo2,myCampo,campoIA);
                    if ( controllo(V,campoIA) == 0 ) {
                        printf ( "\n\n\n\n\nHAI VINTO!\n\n" );
                        printf ( "\n\tLa precisione di %s è stata del %3.2f%%\n\n", nick,  precisione ( M, i ) );
                        break;
                    }
                    printf ( "\n\nLa mossa spetta al nemico" );
                    autoAttacco(lett,myCampo);
                    affondaNave(myCampo);
                    updateCampo(myCampo2,myCampo,campoIA);
                    if ( controllo(V,myCampo) == 0 ) {
                        printf ( "\n\n\n\n\nHai perso!\n\n" );
                        printf ( "\n\tLa precisione del nemico è stata del %3.2f%%\n\n", nick,  precisione ( M, i ) );
                        break;
                    }
                }
            }


    printf ( "\n\n\n\tScopriamo le carte!\n\n\tQuesti sono i campi con la disposizione delle navi:\n\n\tQuesto è il tuo campo\t\t\t\t\tQuesto è il campo nemico\n" );
    for( i = 0; i < Y; i ++ ) {
        printf ( "\n\t%2d", i + 1 );
        for(j=0;j<X;j++){
            visualeAssoluta (i, j, myCampo2 );
        }
        printf ( "\t\t\t\t\t%2d", i + 1);
        for ( j = 0; j < X; j ++ ) {
            visualeAssoluta ( i, j, campoIA2 );
        }
    }
    printf( "\n\t   A B C D E F G H I J\t\t\t\t\t   A B C D E F G H I J\n\n");


    return 0;
}
void azzeraCampo ( int campo[Y][X] ){
    int i;
    int j;
    for ( i = 0; i < X; i ++ ) {
        for ( j = 0; j < Y; j ++ ){
            campo[i][j] = 0;
        }
    }
}
void visualeAssoluta ( int i, int j, int campo[Y][X] ) {
    if ( campo[i][j] == 0 ) printf ( " -" );//acqua
    else if ( campo[i][j] == 1 ) printf ( " O" );//colpita
}
void visualeRelativa ( int i, int j, int campo[Y][X] ) {
    if ( campo[i][j] == 2 ) printf ( " -" );//acqua
    else if ( campo[i][j] == 3 ) printf ( " O" );//colpita
    else if( campo[i][j] == 4 ) printf ( " X" );//affondata
    else printf ( " ?" );//campo coperto
}

void updateCampo ( int campo1[Y][X], int campo2[Y][X], int campo3[Y][X] ) {
    int i;
    int j;

    printf ( "\n\n" );
    printf ( "\t la tua mappa\t\t\t\t\t\t\t visuale nemica\t\t\t\t\t\t\t mappa nemica" );
    for( i = 0; i < X; i ++ ) {
        printf ( "\n\t%2d", i + 1 );
        for( j = 0; j < Y; j ++){
            visualeAssoluta (i, j, campo1 );
        }
        printf ( "\t\t\t\t\t%2d", i + 1 );
        for ( j = 0; j < X; j ++ ){
            visualeRelativa ( i, j, campo2 );
        }
        printf ( "\t\t\t\t\t%2d", i + 1 );
        for ( j = 0; j < X; j ++ ) {
            visualeRelativa ( i, j, campo3 );
        }

    }
    printf( "\n\t   A B C D E F G H I J\t\t\t\t\t   A B C D E F G H I J\t\t\t\t\t   A B C D E F G H I J" );
}

int genCoordManX ( char lett[] ) {
    char c;
    int i;
    do {
        printf ( "\t\tcoordinata alfanuemrica\n\t\t\tx = " );
        scanf (" %c", &c );
    }while ( ( c < 65 || c > 74 ) && ( c < 97 || c > 106 ) );

    if ( c > 97 || c < 106 ) {
        c = toupper(c);
    }
    for ( i = 0 ; i < X ; i ++ ) {
        if ( c == lett[i] ) break;
    }
    return i;
}
int genCoordManY ( char lett[Y] ) {
    int y;
    do {
        printf ( "\t\tcoordinata numerica\n\t\t\ty = " );
        scanf ("%d", &y );
    }while ( y < 1 || y > 10 );
    y = y - 1;
    return y;
}
int controlloSchieraTop ( int d, int x, int y, int yo, int campo[Y][X] ) {
    int errore = 0;
    int i;
    for( i = 0; i < d; i ++ ) {
        if(campo[y][x + 1] == 0 && campo[y][x - 1] == 0 && campo[y][x] == 0 && campo[yo + 1][x - 1] == 0 && campo[yo + 1][x] == 0 && campo[yo + 1][x + 1] == 0 && campo[yo - d][x - 1] == 0 && campo[yo - d][x] == 0 && campo[yo - d][x + 1] == 0 ) campo[y][x] = 1;
        else{
            errore = 1;
            break;
        }
        y--;
    }
    return errore;
}
int controlloSchieraBot ( int d, int x, int y, int yo, int campo[Y][X] ) {
    int errore = 0;
    int i;
    for ( i = y; i < d + y; i ++ ) {
        if ( campo[i][x + 1] == 0 && campo[i][x] == 0 && campo[i][x - 1] == 0 && campo[yo - 1][x - 1] == 0 && campo[yo - 1][x] == 0 && campo[yo - 1][x + 1] == 0 && campo[yo + d][x - 1] == 0 && campo[yo + d][x] == 0 && campo[yo + d][x + 1] == 0 ) campo[i][x] = 1;
        else{
            errore = 1;
            break;
        }
    }
    return errore;
}
int controlloSchieraDex ( int d, int x, int y, int xo, int campo[Y][X] ) {
    int errore = 0;
    int i;
    for ( i = x; i < d + x; i ++ ) {
        if ( campo[y + 1][i] == 0 && campo[y][i] == 0 && campo[y - 1][i] == 0 && campo[y - 1][xo - 1] == 0 && campo[y][xo - 1] == 0 && campo[y + 1][xo - 1] == 0 && campo[y - 1][xo + d] == 0 && campo[y][xo + d] == 0 && campo[y + 1][xo + d] == 0 ) campo[y][i] =1;
        else{
            errore = 1;
            break;
        }
    }
    return errore;
}
int controlloSchieraSin ( int d, int x, int y, int xo, int campo[Y][X] ) {
    int errore = 0;
    int i;
    for ( i = 0; i < d; i ++ ) {
        errore = 0;
        if ( campo[y + 1][x] == 0 && campo[y][x] == 0 && campo[y - 1][x] == 0 && campo[y + 1][xo + 1] == 0 && campo[y][xo + 1] == 0 && campo[y - 1][xo + 1] == 0 && campo[y - 1][xo - d] == 0 && campo[y][xo - d] == 0 && campo[y + 1][xo - d] == 0) campo[y][x] = 1;
        else{
            errore = 1;
            break;
        }
        x--;
    }
    return errore;
}
void orientaNaveUpMan ( char lett[], int d, int campo[Y][X] ) {
    int x;
    int y;
    int yo;
    do{

        x = genCoordManX(lett);
        do {
            y = genCoordManY(lett);
            yo = y;
        }while ( y < d - 1 );

    }while ( controlloSchieraTop ( d, x, y, yo, campo ) != 0 );
}
void orientaNaveDexMan ( char lett[], int d, int campo[Y][X] ) {
    int x;
    int y;
    int xo;
    do{
        do {
            x = genCoordManX(lett);
            xo = x;
        }while( x > X - d );
        y=genCoordManY(lett);
    }while ( controlloSchieraDex ( d, x, y, xo, campo ) != 0 );
}


void orientaNaveBotMan ( char lett[], int d, int campo[Y][X] ){
    int x;
    int y;
    int yo;
    do{
        x = genCoordManX ( lett );
        do{
            y = genCoordManY ( lett );
            yo = y;
        }while( y > Y - d );
    }while ( controlloSchieraBot ( d, x, y, yo, campo ) != 0 );
}
void orientaNaveSinMan ( char lett[], int d, int campo[Y][X] ) {
    int x;
    int y;
    int xo;
    do{
        do{
            x = genCoordManX(lett);
            xo = x;
        }while ( x < d - 1 );
        y = genCoordManY(lett);
    }while ( controlloSchieraSin ( d, x, y, xo, campo ) != 0 );
}
void orientaNaveUp (int d, int campo[Y][X] ) {
    int x;
    int y;
    int yo;
    do{
        x = rand() % 10;
        do{
            y = rand() % 10;
            yo = y;
        } while ( y < d - 1 );
    }while ( controlloSchieraTop ( d, x, y, yo, campo ) != 0 );
}
void orientaNaveDex (int d, int campo[Y][X]) {
    int x;
    int y;
    int xo;
    do {
        y = rand()%10;
        do {
            x = rand()%10;
            xo = x;
        }while ( x > X - d );
    }while ( controlloSchieraDex ( d, x, y, xo, campo ) != 0 );
}
void orientaNaveBot (int d, int campo[Y][X]){
    int x;
    int y;
    int yo;
    int errore;
    do{
        errore = 0;
        x = rand() % 10;
        do {
            y = rand() % 10;
            yo = y;
        }while ( y > Y - d );
    }while ( controlloSchieraBot ( d, x, y, yo, campo ) != 0 );
}
void orientaNaveSin (int d, int campo[Y][X] ) {
    int x;
    int y;
    int xo;
    do{
        do{
            x = rand() % 10;
            xo = x;
        }while(x < d - 1);
        y = rand() % 10;
    }while ( controlloSchieraSin ( d, x, y, xo, campo ) != 0 );
}
int controllo ( int h, int campo[Y][X] ) {
    int i;
    int j;
    int k;
    int errore;
    errore=0;
    k=0;
    for ( i = 0; i < X; i ++ ) {
        for ( j = 0; j < Y; j ++ ) {
            if ( campo[i][j] == h ) k++;
        }
    }
    if ( k != M ) errore = 1;
    return errore;
}
void generaNave ( int d, int campo[Y][X] ) {
    int dir;
    dir = 1 + rand() % 4;
    if ( dir == 1 ) {
        orientaNaveUp(d,campo);
    }
    else if ( dir == 2 ) {
        orientaNaveDex(d,campo);
    }
    else if ( dir == 3 ) {
        orientaNaveBot(d,campo);
    }
    else{
        orientaNaveSin(d,campo);
    };
}
int controlloAttacco ( int x, int y, int campo[Y][X] ) {
    int errore;
    errore=0;
    if ( campo[y][x] != 0 && campo[y][x] != 1 ) errore = 1;
    return errore;
}
void esitoAttacco ( int x, int y,char lett[], int campo[Y][X] ) {
    if ( campo[y][x] == 0 ) {
        campo[y][x] = 2;
        printf ( "\n\n\tL'attacco è andato a segno\n\t\tAcqua nel punto %d%c\n", y + 1, lett[x] );
    }
    else if ( campo[y][x] == 1 ) {
        campo[y][x] = 3;
        printf ( "\n\n\tL'attacco è andato a segno\n\t\tNave colpita nel punto %d%c\n", y + 1, lett[x] );
    }
}
void attacco ( char lett[X], int campo[Y][X] ) {
    int x;
    int y;
    printf ( "\n\n\tInserire coordinate:\n" );
    do{
        x = genCoordManX(lett);
        y = genCoordManY(lett);
        if ( controlloAttacco ( x, y, campo ) != 0 ) printf( "\nCilecca!\n\n\tReinserire coordinate:\n" );
        else{
            esitoAttacco(x,y,lett,campo);
            break;
        }
    }while ( controlloAttacco(x,y,campo) != 0 );
}
void autoAttacco ( char lett[], int campo[Y][X] ) {
    int x;
    int y;
    if ( attaccoForzato(lett,campo) == 0 ) {
        do{
            x = rand() % 10;
            y = rand() % 10;
        } while ( controlloAttacco(x,y,campo) != 0 );
        esitoAttacco(x,y,lett,campo);
    }
}
void copiaCampo ( int campo1[Y][X], int campo2 [Y][X] ) {
    int i;
    int j;
    for (i = 0; i < Y; i ++ ) {
        for ( j = 0; j < X; j ++ ) {
            campo2[i][j] = campo1[i][j];
        }
    }
}
void affondaNave ( int campo[Y][X] ) {
    int i;
    int j;

    for ( i = 0; i < X; i ++ ) {
        for ( j = 0; j < Y; j ++ ) {
            //affonda lancia
            if(campo[i][j] == 3 ) {

                if ( ( campo[i + 1][j] == 0 || campo[i + 1][j] == 2 ) && (campo[i - 1][j] == 0 || campo[i - 1][j] == 2 ) && (campo[i][j - 1] == 0 || campo[i][j - 1] == 2 ) && ( campo[i][j + 1] ==0 || campo[i][j + 1] == 2 ) ) {
                    campo[i][j] = 4;
                    printf ( "\n\n\tLancia affondata!\n\n" );
                }
                //affonda torpediniere
                //up
                else if ( campo[i + 1][j] == 3 && (campo[i + 2][j] == 0 || campo[i + 2][j] == 2 ) && ( campo[i - 1][j] == 0 || campo[i - 1][j] == 2 ) ) {
                    campo[i][j] = 4;
                    campo[i + 1][j] = 4;
                    printf ( "\n\n\tTorpediniere affondato!\n\n" );
                }

                //bot
                else if(campo[i - 1][j] == 3 && (campo[i - 2][j] == 0 || campo[i - 2][j] == 2 ) && ( campo[i + 1][j] == 0 || campo[i + 1][j] == 2 ) ) {
                    campo[i][j] = 4;
                    campo[i - 1][j] = 4;
                    printf ( "\n\n\tTorpediniere affondato!\n\n" );
                }
                //right
                else if ( campo[i][j + 1] == 3 && (campo[i][j + 2] == 0 || campo[i][j + 2] == 2 ) && ( campo[i][j - 1] == 0 || campo[i][j - 1] == 2 ) ) {
                    campo[i][j] = 4;
                    campo[i][j + 1] = 4;
                    printf ( "\n\n\tTorpediniere affondato!\n\n" );
                }
                //left
                else if ( campo[i][j - 1] == 3 && ( campo[i][j - 2] == 0 || campo[i][j - 2] == 2 ) && ( campo[i][j + 1] == 0 || campo[i][j + 1] == 2 ) ) {
                    campo[i][j] = 4;
                    campo[i][j - 1] = 4;
                    printf ( "\n\n\tTorpediniere affondato!\n\n" );
                }
                //affonda sottomarino
                //up
                else if ( campo[i + 1][j] == 3 && campo[i + 2][j] == 3 ) {
                    campo[i][j] = 4;
                    campo[i + 1][j] = 4;
                    campo[i + 2][j] = 4;
                    printf ( "\n\n\tSottomarino affondato!\n\n" );
                }
                //bot
                else if ( campo[i - 1][j] == 3 && campo[i + 2][j] == 3 ) {
                    campo[i][j] = 4;
                    campo[i - 1][j] = 4;
                    campo[i - 2][j] = 4;
                    printf ( "\n\n\tSottomarino affondato!\n\n" );
                }
                //right
                else if ( campo[i][j + 1] == 3 && campo[i][j + 2] == 3 ) {
                    campo[i][j] = 4;
                    campo[i][j + 1] = 4;
                    campo[i][j + 2] = 4;
                    printf ( "\n\n\tSottomarino affondato!\n\n" );
                }
                //sini
                else if ( campo[i][j - 1] == 3 && campo[i][j - 2] == 3) {
                    campo[i][j] = 4;
                    campo[i][j - 1] = 4;
                    campo[i][j - 2] = 4;
                    printf ( "\n\n\tSottomarino affondato!\n\n" );
                }
            }
        }
    }
}
//implementazione dei ragionamenti logici applicati all'algoritmo nel gioco della battaglia navale
int attaccoForzato ( char lett[], int campo[Y][X] ) {
    int top;
    int dex;
    int bot;
    int sin;
    //variabili di lavoro: stato caselle adiacenti: 0:coperta;1:ignora;2:studia;
    top = 0;
    dex = 0;
    bot = 0;
    sin = 0;
    int dir;//scelta casella casuale
    int check;
    check = 0;
    int i;
    int j = 0;
    for ( i = 0; i < Y; i ++ ) {
        for ( j = 0; j < X; j ++ ) {
            //identifica un punto di riferimento come nave colpita ma non affondata e interrompe il ciclo
            if ( campo[i][j] == 3 ){
                check = 1;
                break;
            }
        }
        if ( check == 1 ) break;
    }

    if ( check == 0 ) return check;

    //salvati virtualmente gli indici con il break si studiano le caselle adiacenti
    //quelle da ignorare: se una o più caselle adiacenti scoperte uguali a 2 o risulta che il punto di riferimento sia adiacente ad uno o più bordi allora ignora prima le caselle marginali con i bordi e quelle pari a 2 affinchè: .1 non vengano riattaccate e .2 permettano di mirare un attacco coeso e logico per affondare una nave
    //quelle da considerare: se una o più caselle adiacenti scoperte uguali a 3 allora considararle per attaccare applicando un algoritmo atto ad affondare la nave attaccata
    printf ( "\n\nAttacco forzato!\n" );
    //(casella inesistente e quindi da ignorare se il punto di riferimento è adiacente ad un marine)
    if ( campo[i-1][j] == 2 || i == 0 ) top = 1; //casella top inesistente o acqua scoperta = ignora
    else if ( campo[i-1][j] == 3 ) top = 2; //casella top nave scoperta = studia per attacco mirato
    if ( campo[i][j+1] == 2 || j == 9 ) dex = 1; //casella dex inesistente o acqua scoperta = ignora
    else if ( campo[i][j+1] == 3 ) dex = 2; //casella dex nave scoperta = studia per attacco mirato
    if ( campo[i+1][j] == 2 || i == 9 ) bot = 1; //casella bot inesistente o acqua scoperta = ignora
    else if ( campo[i+1][j] == 3 ) bot = 2; //casella bot nave scoperta = studia per attacco mirato
    if ( campo[i][j-1] == 2 || j == 0 ) sin = 1; //casella sin inesistente o acqua scoperta = ignora
    else if ( campo[i][j-1] == 3 ) sin = 2; //casella sin nave scoperta = studia per attacco mirato




    //possibili casistiche di attacco per affondare una nave di cui si ha un punto di riferimento come nave colpita ma non affondata
    //se adiacente al bordo superiore del mio punto di riferimento ho una nave colpita ma non affondata allora procedo ad attaccare il punto adiacente al bordo base  se questo esiste e non è una casella acqua scoperta di stato 2
    if ( top == 2 && bot == 0  ) {
        i = i + 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    else if ( top == 2 && ( campo[i - 2][j] == 0 || campo[i - 2][j] == 1 ) && i >= 2 ) {
        i = i - 2;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //se adiacente al bordo destro del punto di riferimento c'è una cella di stato 3 allora procedo ad attaccare la cella adiacente al bordo opposto, quello sinistro, solo se questa esiste e non è una cella di stato 2 (esiste solo se il punto di riferimento non è adiacente ad uno o piu dei bordi)
    if ( dex == 2 && sin == 0 ) {
        j = j - 1;
        esitoAttacco(j,i,lett,campo);
        return check;

    }
    else if ( dex == 2 && ( campo[i][j + 2] == 0 || campo[i][j + 2] == 1 ) && j <= 7  ) {
        j = j + 2;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //se la cella adiacente al bordo base del punto di riferimento è una nave colpita e non affondata allora attacco la cella adiacente al lato opposto solo se esiste e non è di stato due
    if ( bot == 2 && top == 0 ) {
        i = i - 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    else if ( bot == 2 && ( campo[i + 2][j] == 0 || campo[i + 2][j] == 1 ) && i <= 7 ) {
        i = i + 2;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //la cella adiacente al amrgine sinistro è tre quindi se la cella adiacente al lato opposto esiste e non è di stato 2 la attacco
    if ( sin == 2 && dex == 0 ) {
        j = j + 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    else if ( sin == 2 && ( campo[i][j - 2] == 0 || campo[i][j - 2] == 1 ) && j >= 2 ) {
        j = j - 2;
        esitoAttacco(j,i,lett,campo);
        return check;
    }




    //tutte le caselle adiacenti sono coperte quindi ne attacco una a caso
    if ( top == 0 && dex == 0 && bot == 0 && sin == 0 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 4;
        //top
        if ( dir == 1 ) i = i - 1;
        //dex
        else if ( dir == 2 ) j = j + 1;
        //bot
        else if ( dir == 3 ) i = i + 1;
        //sin
        else j = j - 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }





    //tutte le caselle adiacenti sono bersagliabili tranne quella adiacente verso l'alto quindi attacco a caso una fra tutte escludendo questa
    if ( top == 1 && dex == 0 && bot == 0 && sin == 0 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 3;
        //dex
        if ( dir == 1 ) j = j + 1;
        //bot
        else if (dir == 2) i = i + 1;
        //sin
        else j = j - 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //tutte le caselle adiacenti sono bersagliabili tranne quella adiacente verso destra quindi attacco a caso una fra tutte escludendo questa
    if ( top == 0 && dex == 1 && bot == 0 && sin == 0 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 3;
        //top
        if ( dir == 1 ) i = i - 1;
        //sin
        else if ( dir == 2 ) j = j - 1;
        //bot
        else  i = i + 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //tutte le caselle adiacenti sono bersagliabili tranne quella adiacente verso il basso quindi attacco a caso una fra tutte escludendo questa
    if ( top == 0 && dex == 0 && bot == 1 && sin == 0 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 3;
        //top
        if ( dir == 1 ) i = i - 1;
        //dex
        else if ( dir == 2 ) j = j + 1;
        //sin
        else j = j - 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //tutte le caselle adiacenti sono bersagliabili tranne quella adiacente verso sinistra quindi attacco a caso una fra tutte escludendo questa
    if ( top == 0 && dex == 0 && bot == 0 && sin == 1 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 3;
        //top
        if ( dir == 1 ) i = i - 1;
        //dex
        else if ( dir == 2 ) j = j + 1;
        //bot
        else i = i + 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }







    //casistiche di attacco con due celle adiacenti di stato 2
    //se top e dex adiacenti e di stato due con bot e sin bersagliabili perchè identificati di stato 0
    if ( top == 1 && dex == 1 && bot == 0 && sin == 0 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 2;
        //bot
        if ( dir == 1 ) i = i + 1;
        //sin
        else j = j - 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //se top e bot adiacenti e di stato due con bot e sin bersagliabili perchè identificati di stato 0
    if ( top == 1 && dex == 0 && bot == 1 && sin == 0 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 2;
        //dex
        if ( dir == 1 ) j = j + 1;
        //sin
        else j = j - 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //se top e sin adiacenti e di stato due con bot e sin bersagliabili perchè identificati di stato 0
    if ( top == 1 && dex == 0 && bot == 0 && sin == 1 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 2;
        //dex
        if ( dir == 1 ) j = j + 1;
        //bot
        else i = i + 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //se dex e bot adiacenti e di stato due con bot e sin bersagliabili perchè identificati di stato 0
    if ( top == 0 && dex == 1 && bot == 1 && sin == 0 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 2;
        //sin
        if ( dir == 1 ) j = j - 1;
        //top
        else i = i - 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }

    //se bot e sin adiacenti e di stato due con bot e sin bersagliabili perchè identificati di stato 0
    if ( top == 0 && dex == 0 && bot == 1 && sin == 1 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 2;
        //dex
        if ( dir == 1 ) j = j + 1;
        //top
        else i = i - 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }

    //se dex e sin adiacenti e di stato due con bot e sin bersagliabili perchè identificati di stato 0
    if ( top == 0 && dex == 1 && bot == 0 && sin == 1 ) {
        //scelta casuale casella da attaccare
        dir = 1 + rand() % 2;
        //top
        if ( dir == 1 ) i = i - 1;
        //bot
        else i = i + 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }






    //casistiche di attacco con 3 celle adiacenti di stato 2
    //posso attaccare solo top
    if ( top == 0 && dex == 1 && bot == 1 && sin == 1 ) {
        i = i - 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //posso attaccare solo dex
    if ( top == 1 && dex == 0 && bot == 1 && sin == 1 ) {
        j = j + 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //posso attaccare solo bot
    if ( top == 1 && dex == 1 && bot == 0 && sin == 1 ) {
        i = i + 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    //posso attaccare solo sin
    if ( top == 1 && dex == 1 && bot == 1 && sin == 0 ) {
        j = j - 1;
        esitoAttacco(j,i,lett,campo);
        return check;
    }
    return check;
}

float precisione ( int d, int turni ){
    float mira;
    mira = (float)   d /  turni  ;
    mira = mira * 100;
    return mira;
}
