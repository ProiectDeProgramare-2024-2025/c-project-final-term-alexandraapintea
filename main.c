#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_CAMERE 5 //hotelul are maxim 5 camere
#define RED     "\033[1;31m" //definim culori pentru a le putea utiliza mai incolo
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
#define RESET   "\033[0m"
#define LIGHT_CYAN "\033[96m"
#define ORANGE "\033[38;5;208m"
// Structura pentru camere
typedef struct {
    char tip[20];             // Tipul camerei
    int etaj;                 // Etajul
    int wiFi;                 // 0 - nu, 1 - da
    int parcare;              // 0 - nu, 1 - da
    float pret_pe_noapte;     // Prețul pe noapte
    int ora_disponibilitate;  // Ora la care camera este disponibilă
} Camera;
// Structura pentru rezervare
typedef struct {
    char tip[20];      // Tipul camerei rezervate
    int nr_persoane;   // Numar de persoane
    int zile;          // Număr de zile
    float pret_total;  // Preț total
    int activa;        // 0 - rezervare inactiva, 1 - rezervare activa
} Rezervare;
Rezervare rezervari[MAX_CAMERE] = {0}; // Inițializăm rezervarile ca fiind inactive
// Prototipuri funcții
void rezerva_camera(Camera camere[], int nr_camere);
void adauga_camera(Camera camere[], int *nr_camere, Rezervare rezervari[]);
void anuleaza_rezervare(Camera camere[], int nr_camere, Rezervare rezervari[]);
void clear_screen();
void cautare_camere(Camera camere[], int nr_camere, char tip[], int etaj, int wiFi, int parcare, float max_pret, int ora);
void modifica_pret(Camera camere[], int nr_camere, int index);
void modifica_facilitati(Camera camere[], int nr_camere, int index);
void sterge_camera(Camera camere[], int *nr_camere);
int autentificare_admin();
void afiseaza_camere(Camera camere[], int nr_camere);
void cautare_dupa_tip(Camera camere[], int nr_camere, char tip[]);
void salveaza_date(Camera camere[], int nr_camere, Rezervare rezervari[]);
void citeste_date(Camera camere[], int *nr_camere, Rezervare rezervari[]);
void afiseaza_rezervari(Camera camere[], int nr_camere, Rezervare rezervari[]);
int valid_tip(char tip[]) //verifica daca un char este valid adica daca contine doar litere
{
    for (int i = 0; tip[i] != '\0'; i++) //parcurge stringul
        {
        if (!isalpha(tip[i])) //daca nu e litera
        {
            return 0;  // Nu e litera → invalid
        }
    }
    return 1; //altfel returnam 1 ca inseamna ca e litera
}
void afiseaza_rezervari(Camera camere[], int nr_camere, Rezervare rezervari[]) {
    clear_screen();
    int gasite = 0;
    printf("=== " MAGENTA "REZERVARI ACTIVE" RESET " ===\n\n");
    for (int i = 0; i < nr_camere; i++)
        {
        if (rezervari[i].activa == 1)  //daca camera curenta e rezervata o afisez
        {
            printf("Index: %d\n", i);
            printf("Tip Camera: " RED "%s\n" RESET, rezervari[i].tip);
            printf("Numar persoane: " CYAN "%d\n" RESET, rezervari[i].nr_persoane);
            printf("Numar zile: " BLUE "%d\n" RESET, rezervari[i].zile);
            printf("Pret total: " YELLOW "%.2f RON\n" RESET, rezervari[i].pret_total);
            printf("Ora Check-in: " GREEN "%02d:00\n" RESET, camere[i].ora_disponibilitate);
            printf("\n");
            gasite = 1;
        }
    }
    if (!gasite) {
        printf(RED "Nu exista rezervari active" RESET " in acest moment.\n\n");
    }
    printf("Apasa Enter pentru a continua...");
    getchar();
}
//Funcția pentru a adauga o camera
void adauga_camera(Camera camere[], int *nr_camere, Rezervare rezervari[])
{
    clear_screen();
    if (*nr_camere >= MAX_CAMERE) //daca nr de camere la momentul actual este deja atins, adica deja exista 5 camere
        {
        printf(RED "Nu se mai pot adauga camere" RESET ". Maximul de %d camere a fost atins!\n", MAX_CAMERE);
        printf("Apasa Enter pentru a continua...");
        getchar(); getchar();
        return;
        }
    //altfel inseamna ca pot adauga camera, pt ca maximul inca nu a fost atins
    Camera noua; //declar o camera noua de tipul structurii
    char buffer_tip[100]; //citesc in buffer tipul
    while (1)
        {
        printf("Introduceti tipul camerei: ");
        fgets(buffer_tip, sizeof(buffer_tip), stdin); //citim tipul intr-un buffer
        buffer_tip[strcspn(buffer_tip, "\n")] = '\0'; // eliminăm '\n'
        //Verificam validitatea
        if (strlen(buffer_tip) > 0 && valid_tip(buffer_tip)) //tipul camerei trebuie sa contina doar litere
        {
            strcpy(noua.tip, buffer_tip); //copiez tipul
            break;
        } else { printf(RED "Tip invalid" RESET "! Introduceti doar litere fara cifre sau simboluri.\n");}
       }
    char buffer[100];
    while (1)
    {
        printf("Introduceti etajul (Hotelul are 10 etaje!): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0; // eliminam newline
        int valid = 1;
        for (int i = 0; buffer[i]; i++)
        {
            if (!isdigit(buffer[i])) //daca nu e cifra ceea ce am introdus
            {
                valid = 0;
                break;
            }
        }
    if (valid && sscanf(buffer, "%d", &noua.etaj) == 1 && noua.etaj >= 0 && noua.etaj <= 10) //daca am introdus o cifra intre 0 si 10 este valid
    {
        break; // Etaj corect
    } else {printf(RED "Etaj invalid" RESET "! Introduceti un numar intre 0 si 10.\n");}
    }
    // === VALIDARE WiFi ===
    char buffer_wifi[100];
    while (1)
    {
        printf("Are Wi-Fi? (1 = Da, 0 = Nu): ");
        fgets(buffer_wifi, sizeof(buffer_wifi), stdin);
        buffer_wifi[strcspn(buffer_wifi, "\n")] = 0;
        if (strlen(buffer_wifi) == 1 && (buffer_wifi[0] == '0' || buffer_wifi[0] == '1'))
        {
            noua.wiFi = buffer_wifi[0] - '0';
            break;
        }
        printf("Valoare invalida! Introduceti 1 (Da) sau 0 (Nu): ");
    }
    // === VALIDARE Parcare ===
    char buffer_parcare[100];
    while (1)
    {
        printf("Are parcare? (1 = Da, 0 = Nu): ");
        fgets(buffer_parcare, sizeof(buffer_parcare), stdin);
        buffer_parcare[strcspn(buffer_parcare, "\n")] = 0;
        if (strlen(buffer_parcare) == 1 && (buffer_parcare[0] == '0' || buffer_parcare[0] == '1'))
        {
            noua.parcare = buffer_parcare[0] - '0';
            break;
        }
        printf("Valoare invalida! Introduceti 1 (Da) sau 0 (Nu): ");
    }
    int prima_incercare = 1;
    while (1)
    {
    if (prima_incercare)
    {
        printf("Introduceti pretul pe noapte (intre 100 si 1000 RON/noapte): ");
        prima_incercare = 0;
    }
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    // Dacă nu s-a introdus nimic (doar ENTER), repetăm promptul FĂRĂ mesaj de eroare
    if (strlen(buffer) == 0) continue;
    int puncte = 0, valid = 1;
    for (int i = 0; buffer[i]; i++)
    {
        if (buffer[i] == '.')
        {
            puncte++;
            if (puncte > 1)
            {
                valid = 0;
                break;
            }
        } else if (!isdigit(buffer[i]))
        {
            valid = 0;
            break;
        }
    }
    if (valid && sscanf(buffer, "%f", &noua.pret_pe_noapte) == 1 &&
        noua.pret_pe_noapte >= 100 && noua.pret_pe_noapte <= 1000)
        {
        break;} else {printf(RED "Pret invalid" RESET "! Introduceti o valoare intre 100 si 1000 RON/noapte.\n");}
    }
    char buffer_ora[100];
    int prima_incercare_ora = 1;
    while (1)
    {
    if (prima_incercare_ora)
    {
        printf("Introduceti ora disponibilitatii (ex: 14 pentru 14:00): ");
        prima_incercare_ora = 0;
    }
    fgets(buffer_ora, sizeof(buffer_ora), stdin);
    buffer_ora[strcspn(buffer_ora, "\n")] = 0;
    int valid = 1;
    for (int i = 0; i < strlen(buffer_ora); i++)
    {
        if (!isdigit(buffer_ora[i]))
        {
            valid = 0;
            break;
        }
    }
    if (!valid || sscanf(buffer_ora, "%d", &noua.ora_disponibilitate) != 1 ||
        noua.ora_disponibilitate < 0 || noua.ora_disponibilitate > 23)
        {
        printf(RED "Ora invalida" RESET "! Introduceti o ora intre 0 si 23:\n");
        printf("Introduceti ora disponibilitatii (ex: 14 pentru 14:00): ");
        continue;
        }
    break; // ora validă
}
    camere[*nr_camere] = noua; //de ex camere[5]=noua
    rezervari[*nr_camere].activa = 0; //o marcam ca fiind nerezervata
    (*nr_camere)++; //crestem nr de camere ale hotelului
    salveaza_date(camere, *nr_camere, rezervari); //salvam datele noi
    printf("Camera a fost adaugata cu succes!\n");
    printf("\nApasa Enter pentru a continua...");
    getchar();
    getchar();
}
//functie de rezervare a unei camere
void rezerva_camera(Camera camere[], int nr_camere)
{
    clear_screen();
    char buffer[100];
    int index, nr_persoane, zile;
    while (1)
        {
        printf("Introduceti indexul camerei pe care doriti sa o rezervati (0 - %d): ", nr_camere - 1);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // elimină '\n'
        // Ignoră dacă userul apasă doar Enter
        if (strlen(buffer) == 0)
        {
            continue;
        }
        // Verifică dacă toate caracterele sunt cifre
        int valid = 1;
        for (int i = 0; i < strlen(buffer); i++)
        {
            if (!isdigit(buffer[i]))
            {
                valid = 0;
                break;
            }
        }
        if (!valid || sscanf(buffer, "%d", &index) != 1 || index < 0 || index >= nr_camere)
        {
            printf(RED "Index invalid" RESET "! Introduceti un numar intre 0 si %d:\n", nr_camere - 1);
            continue;
        }
        break;  // input corect
    }
    if (rezervari[index].activa == 1)
    {
        printf("Aceasta camera este deja rezervata!\n");
        printf("Apasa Enter pt a continua...");
        getchar();
        return;
    }
    // === VALIDARE PERSOANE ===
    while (1)
    {
        printf("Introduceti numarul de persoane (maxim 5): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        int valid = 1;
        for (int i = 0; buffer[i]; i++) {
            if (!isdigit(buffer[i])) {
                valid = 0;
                break;
            }
        }

        if (valid && sscanf(buffer, "%d", &nr_persoane) == 1 && nr_persoane >= 1 && nr_persoane <= 5) {
            break;
        } else {
            printf(RED "Numar invalid" RESET "! Introduceti o valoare intre 1 si 5.\n");
        }
   }
// === VALIDARE ZILE ===
    while (1)
    {
        printf("Introduceti numarul de zile (maxim 10): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        int valid = 1;
        for (int i = 0; buffer[i]; i++)
            {
            if (!isdigit(buffer[i]))
            {
                valid = 0;
                break;
            }
           }
        if (valid && sscanf(buffer, "%d", &zile) == 1 && zile >= 1 && zile <= 10) {
            break;
        } else {
            printf(RED "Numar invalid!" RESET " Introduceti o valoare intre 1 si 10.\n");
        }
    }
    rezervari[index].activa = 1;
    strncpy(rezervari[index].tip, camere[index].tip, sizeof(rezervari[index].tip) - 1);
    rezervari[index].tip[sizeof(rezervari[index].tip) - 1] = '\0';
    rezervari[index].nr_persoane = nr_persoane;
    rezervari[index].zile = zile;
    rezervari[index].pret_total = zile * camere[index].pret_pe_noapte;
    printf("Rezervarea a fost efectuata cu succes!\n"); //mesaj sugestiv
    printf(MAGENTA "Detalii rezervare" RESET ":\n");
    printf("Tip Camera: %s\n", rezervari[index].tip);
    printf("Numar persoane: %d\n", rezervari[index].nr_persoane);
    printf("Numar zile: %d\n", rezervari[index].zile);
    printf("Pret total: %.2f RON\n", rezervari[index].pret_total);
    printf("\nApasa Enter pentru a continua...");
    getchar();
    salveaza_date(camere, nr_camere, rezervari);
}
// Funcția pentru a anula o rezervare
void anuleaza_rezervare(Camera camere[], int nr_camere, Rezervare rezervari[])
{
    clear_screen();
     int index;
    char buffer[100];
    printf("Introduceti indexul camerei pentru care doriti sa anulati rezervarea (0 - %d): ", MAX_CAMERE - 1);
    while (fgets(buffer, sizeof(buffer), stdin))
        {
        buffer[strcspn(buffer, "\n")] = 0;
        int valid = 1;
        for (int i = 0; buffer[i]; i++)
        {
            if (!isdigit(buffer[i]))
            {
                valid = 0;
                break;
            }
        }
        if (valid && sscanf(buffer, "%d", &index) == 1 && index >= 0 && index < MAX_CAMERE)
            break;
    printf(RED "Index invalid" RESET "! Introduceti un numar intre 0 si %d:\n", MAX_CAMERE - 1);
    printf("Introduceti indexul camerei pentru care doriti sa anulati rezervarea (0 - %d): ", MAX_CAMERE - 1);
       }
    if (!rezervari[index].activa) //daca camera nu era rezervata inainte inseamna ca nu pot sa i anulez rezervarea
        {
        printf("\nAceasta camera nu este rezervata!\n");
        printf("\nApasa Enter pentru a continua...");
        getchar();
        getchar();
        return;
        }
    printf("\nAnulati rezervarea pentru:\n");
    printf("Tip Camera: %s\n", rezervari[index].tip);
    printf("Numar persoane: %d\n", rezervari[index].nr_persoane);
    printf("Numar zile: %d\n", rezervari[index].zile);
    printf("Pret total: %.2f\n", rezervari[index].pret_total);
    // RESETARE COMPLETĂ pt ca am anulat rezervarea deci aceasta are status la rezervare inactiv
    strcpy(rezervari[index].tip, "");
    rezervari[index].nr_persoane = 0;
    rezervari[index].zile = 0;
    rezervari[index].pret_total = 0.0;
    rezervari[index].activa = 0;
    printf("\nRezervarea pentru camera %s a fost anulata cu succes!\n", camere[index].tip);
    printf("\nApasa Enter pentru a continua...");
    getchar();
    getchar();
    salveaza_date(camere, nr_camere, rezervari);  // salvam totul nou
}
// Funcția pentru a șterge ecranul
void clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
// Funcția pentru a căuta camere după diverse criterii
void cautare_camere(Camera camere[], int nr_camere, char tip[], int etaj, int wiFi, int parcare, float max_pret, int ora)
{

    printf("Camere disponibile care se potrivesc criteriilor tale:\n");
    int gasite = 0;
    for (int i = 0; i < nr_camere; i++)
        {
        if ((strlen(tip) == 0 || strcmp(camere[i].tip, tip) == 0) &&
            (etaj == -1 || camere[i].etaj == etaj) &&
            (wiFi == -1 || camere[i].wiFi == wiFi) &&
            (parcare == -1 || camere[i].parcare == parcare) &&
            camere[i].pret_pe_noapte <= max_pret &&
            camere[i].ora_disponibilitate <= ora)
            {
            printf("Tip Camera: %s\n", camere[i].tip);
            printf("Etaj: %d\n", camere[i].etaj);
            printf("Wi-Fi: %s\n", camere[i].wiFi ? "Da" : "Nu");
            printf("Parcare: %s\n", camere[i].parcare ? "Da" : "Nu");
            printf("Pret pe noapte: %.2f\n", camere[i].pret_pe_noapte);
            printf("Ora disponibilitate: %d:00\n\n", camere[i].ora_disponibilitate);
            gasite = 1; //marchez ca am gasit o camera pe baza criteriului
           }
    }
    if (!gasite) {printf("Nu au fost gasite camere care sa se potriveasca criteriilor tale.\n");}
}
// Funcția pentru a modifica prețul unei camere (se primește indexul camerei deja ales)
void modifica_pret(Camera camere[], int nr_camere, int index)
{
    clear_screen();
    char buffer[100];
    float nou_pret;
    int prima_incercare = 1;
    while (1) {
        if (prima_incercare)
            {
            printf("Pretul actual pentru camera %s este: " YELLOW "%.2f RON\n" RESET, camere[index].tip, camere[index].pret_pe_noapte);
            printf("Introduceti noul pret pentru camera %s (intre 100 si 1000 RON/noapte): ", camere[index].tip);
            prima_incercare = 0;
            }
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) == 0) continue; // ENTER fără valoare
        int valid = 1, puncte = 0;
        for (int i = 0; buffer[i]; i++)
        {
            if (buffer[i] == '.')
            {
                puncte++;
                if (puncte > 1)
                {
                    valid = 0;
                    break;
                }
            } else if (!isdigit(buffer[i]))
            {
                valid = 0;
                break;
            }
        }
        if (valid && sscanf(buffer, "%f", &nou_pret) == 1 && nou_pret >= 100 && nou_pret <= 1000)  //daca pretul introdus e valid
        {
            camere[index].pret_pe_noapte = nou_pret; //ii atribui noul pret
            break;
        } else {
            printf(RED "Pret invalid!" RESET " Introduceti un numar intre 100 si 1000.\n");
            printf("Introduceti noul pret pentru camera %s: ", camere[index].tip);
               }
    }
    printf("Pretul pentru camera %s a fost modificat cu succes la %.2f RON!\n", camere[index].tip, camere[index].pret_pe_noapte);
    printf("\nApasa Enter pentru a continua...");
    getchar();
    salveaza_date(camere, nr_camere, rezervari);
}
// Funcția pentru a modifica facilitățile unei camere (se primește indexul camerei deja ales)
void modifica_facilitati(Camera camere[], int nr_camere, int index)
{
    clear_screen();
    char buffer[100];
    // Validare index camera
    while (1)
        {
        printf("Introduceti indexul camerei pentru modificare (0 - %d): ", nr_camere - 1);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        int valid = 1;
        for (int i = 0; buffer[i]; i++)
            if (!isdigit(buffer[i])) valid = 0;
        if (valid && sscanf(buffer, "%d", &index) == 1 && index >= 0 && index < nr_camere)
            break;
        printf(RED "Index invalid" RESET "! Introduceti un numar intre 0 si %d.\n", nr_camere - 1);
        }
    printf("Modificati facilitati pentru camera %s:\n", camere[index].tip);
    printf("1. Wi-Fi (1 pentru Da, 0 pentru Nu)\n");
    printf("2. Parcare (1 pentru Da, 0 pentru Nu)\n");
    char opt_buf[100];
    int optiune;
    // Validare alegere optiune (1 sau 2)
    while (1)
    {
        printf("Alegeti optiunea: ");
        fgets(opt_buf, sizeof(opt_buf), stdin);
        opt_buf[strcspn(opt_buf, "\n")] = 0;
        if (strlen(opt_buf) == 1 && (opt_buf[0] == '1' || opt_buf[0] == '2'))
        {
            optiune = opt_buf[0] - '0';
            break;
        }
        printf(RED "Optiune invalida" RESET "! Alegeti 1 sau 2.\n");
    }
    char val_buf[100];
    int valoare;
    // Validare valoare pentru Wi-Fi sau parcare (0 sau 1)
    while (1)
    {
        if (optiune == 1)
            printf("Doriti Wi-Fi? (1 pentru Da, 0 pentru Nu): ");
        else
            printf("Doriti parcare? (1 pentru Da, 0 pentru Nu): ");
        fgets(val_buf, sizeof(val_buf), stdin);
        val_buf[strcspn(val_buf, "\n")] = 0;
        if (strlen(val_buf) == 1 && (val_buf[0] == '0' || val_buf[0] == '1'))
        {
            valoare = val_buf[0] - '0';
            if (optiune == 1)
                camere[index].wiFi = valoare;
            else
                camere[index].parcare = valoare;
            break;
        }
        printf("Valoare invalida! Introduceti 1 (Da) sau 0 (Nu): ");
    }
    salveaza_date(camere, nr_camere, rezervari);
}
// Funcția pentru a șterge o cameră
void sterge_camera(Camera camere[], int *nr_camere)
{
    clear_screen();
    int index; //indexul camerei de sters
    char buffer[100];
    printf("Introduceti indexul camerei de sters (0 - %d): ", *nr_camere - 1);
    while (fgets(buffer, sizeof(buffer), stdin))
        {
        buffer[strcspn(buffer, "\n")] = 0;
        int valid = 1;
        for (int i = 0; buffer[i]; i++)
            if (!isdigit(buffer[i])) valid = 0;
        if (valid && sscanf(buffer, "%d", &index) == 1 && index >= 0 && index < *nr_camere) //indexul valid
            break;
    printf(RED "Index invalid" RESET "! Introduceti un numar intre 0 si %d:\n", *nr_camere - 1);
    printf("Introduceti indexul camerei de sters (0 - %d): ", *nr_camere - 1);
        }
    // Salvăm numele camerei înainte să o stergem
    char nume_sters[20];
    strcpy(nume_sters, camere[index].tip);
    // Dacă era rezervată, anulăm rezervarea
    if (rezervari[index].activa == 1)
    {
        printf("Camera era rezervata. Stergem si rezervarea asociata.\n");
        rezervari[index].activa = 0;
        strcpy(rezervari[index].tip, "");
        rezervari[index].nr_persoane = 0;
        rezervari[index].zile = 0;
        rezervari[index].pret_total = 0.0;
    }
    // Mutăm camerele și rezervările spre stânga
    for (int i = index; i < *nr_camere - 1; i++)
    {
        camere[i] = camere[i + 1];
        rezervari[i] = rezervari[i + 1];
    }
    // Resetăm ultima rezervare
    rezervari[*nr_camere - 1].activa = 0;
    strcpy(rezervari[*nr_camere - 1].tip, "");
    rezervari[*nr_camere - 1].nr_persoane = 0;
    rezervari[*nr_camere - 1].zile = 0;
    rezervari[*nr_camere - 1].pret_total = 0.0;
    (*nr_camere)--; //scadem din nr de camere
    printf("Camera %s a fost stearsa cu succes.\n", nume_sters);
    salveaza_date(camere, *nr_camere, rezervari);
    printf("Apasa Enter pentru a continua...");
    getchar();
    getchar();
}
// Funcția pentru autentificarea adminului
int autentificare_admin()
{
    char raspuns[1000];
    int incercari = 0;
    printf("Sunteti admin? (Raspundeti cu \033[1;31my\033[0m/\033[1;32mn\033[0m): ");
    while (1)
        {
        fgets(raspuns, sizeof(raspuns), stdin);
        if (raspuns[1] != '\0' && raspuns[1] != '\n')
        {
            printf(RED "Raspuns invalid" RESET "! Te rugam sa raspunzi cu '" RED "y" RESET "' sau '" GREEN "n" RESET "'.\n");
            continue;
        }
        if (raspuns[0] == 'y' || raspuns[0] == 'Y')
        {
            char parola[20];
            while (incercari < 3)
            {
                printf("Introduceti \033[1;34mparola\033[0m de admin: ");
                scanf("%s", parola);
                getchar();
                if (strcmp(parola, "admin") == 0)
                {
                    return 1;
                } else
                {
                    printf("Parola \033[1;31mincorecta\033[0m! Mai aveti %d incercari.\n", 2 - incercari);
                    incercari++;
                }
            }
            printf("Ati depasit numarul de incercari. Programul se va opri.\n");
            exit(0);
        } else if (raspuns[0] == 'n' || raspuns[0] == 'N')
        {
            return 0;
        } //else {
            //printf("Te rugam sa raspunzi cu 'y' sau 'n'.\n");
            //continue;
        //}
    }
}
// Funcția pentru a afișa toate camerele disponibile
void afiseaza_camere(Camera camere[], int nr_camere)
{
    clear_screen();
    if (nr_camere == 0)
        {
    printf("Hotelul nu contine inca nicio camera!\n");
    printf("Apasa orice tasta pentru a continua...\n");
    getchar();
    getchar();
    return;
        }
    printf("Camere disponibile (\033[1;35mNErezervate\033[0m):\n");
    int gasite = 0;
    for (int i = 0; i < nr_camere; i++)
        {
        if (rezervari[i].activa!=1)
        {  // doar dacă NU este rezervată ii afisez pe ecran caracteristicile
                printf("Index: %d\n", i);
        printf("Tip Camera: " RED "%s\n" RESET, camere[i].tip);
        printf("Etaj: " CYAN "%d\n" RESET, camere[i].etaj);
        printf("Wi-Fi: " BLUE "%s\n" RESET, camere[i].wiFi ? "Da" : "Nu");
        printf("Parcare: " GREEN "%s\n" RESET, camere[i].parcare ? "Da" : "Nu");
        printf("Pret pe noapte: " YELLOW "%.2f RON\n" RESET, camere[i].pret_pe_noapte);
        printf("Ora disponibilitate: " ORANGE "%02d:00\n\n" RESET, camere[i].ora_disponibilitate);
        gasite = 1;
        }
    }
    if (!gasite) {
        printf("Toate camerele sunt rezervate momentan.\n");}
    printf("Apasa orice tasta pentru a continua...\n");
    getchar();
    getchar();
}
// Funcția pentru a căuta camere după tip (ex: Standard, Luxe etc.)
void cautare_dupa_tip(Camera camere[], int nr_camere, char tip[])
{
    cautare_camere(camere, nr_camere, tip, -1, -1, -1, 99999.0, 24);
}
// Funcția de salvare a datelor în fișier
void salveaza_date(Camera camere[], int nr_camere, Rezervare rezervari[]) {
    FILE *f = fopen("pp_proiect_programare/hotell.txt", "w"); //deschidem fiiserul in modul de scriere/salvare date
    if (!f) {
        perror("Eroare la deschiderea fisierului pentru scriere");
        return;}
    fprintf(f, "NUMAR CAMERE: %d\n", nr_camere);
    fprintf(f, "=== LISTA CAMERE APARTINAND HOTELULUI ===\n");
    fprintf(f, "%-12s %-6s %-6s %-8s %-12s %s\n",
            "Tip", "Etaj", "WiFi", "Parcare", "Pret/noapte", "Ora disponibilitate"); //scriem antetul
    for (int i = 0; i < nr_camere; i++) {
        fprintf(f, "%-12s %-6d %-6s %-8s %-12.2f %02d:00\n",
                camere[i].tip,
                camere[i].etaj,
                camere[i].wiFi ? "Da" : "Nu",
                camere[i].parcare ? "Da" : "Nu",
                camere[i].pret_pe_noapte,
                camere[i].ora_disponibilitate);
    }
    fprintf(f, "\n=== REZERVARI ACTIVE ===\n");
    fprintf(f, "%-12s %-6s %-9s %-12s %s\n",
            "Tip Camera", "Zile", "Persoane", "Pret total", "Ora Check-in");
    int exista_rezervari = 0;
    for (int i = 0; i < MAX_CAMERE; i++)
        {
        if (rezervari[i].activa == 1) //daca camera curenta e rezervata
        {
            exista_rezervari = 1; //marcam ca avem cel putin o camera rezervata
            fprintf(f, "%-12s %-6d %-9d %-12.2f %02d:00\n",
                    rezervari[i].tip,
                    rezervari[i].zile,
                    rezervari[i].nr_persoane,
                    rezervari[i].pret_total,
                    camere[i].ora_disponibilitate);
        }
    }
    if (!exista_rezervari)
        {
        fprintf(f, "(Nu exista rezervari active in acest moment.)\n");
        }
    fclose(f); //inchidem fisierul
}
// Funcția de citire a datelor din fișier
void citeste_date(Camera camere[], int *nr_camere, Rezervare rezervari[])
{
    FILE *f = fopen("pp_proiect_programare/hotell.txt", "r"); //deschidere fisier pt scriere
    if (!f) return; //verificam daca fisierul s-a deschis cu succes
    char linie[256], wifi_str[10], parcare_str[10];
    int index = 0;
    // Citim numărul de camere
    fgets(linie, sizeof(linie), f);
    sscanf(linie, "NUMAR CAMERE: %d", nr_camere);
    // Sărim peste linia de titlu
    while (fgets(linie, sizeof(linie), f))
        {
        if (strstr(linie, "Tip") != NULL)
            break;
    }
    // Citim camerele
    for (int i = 0; i < *nr_camere; i++) {
        fgets(linie, sizeof(linie), f);
        sscanf(linie, "%s %d %s %s %f %d:%*d",
               camere[i].tip,
               &camere[i].etaj,
               wifi_str,
               parcare_str,
               &camere[i].pret_pe_noapte,
               &camere[i].ora_disponibilitate); //format cu sscanf
        camere[i].wiFi = strcmp(wifi_str, "Da") == 0 ? 1 : 0;
        camere[i].parcare = strcmp(parcare_str, "Da") == 0 ? 1 : 0;
    }
    // Citim până ajungem la REZERVARI ACTIVE
    while (fgets(linie, sizeof(linie), f))
    {
        if (strstr(linie, "REZERVARI ACTIVE") != NULL)
            break;
    }
    // Citim linia de antet din REZERVARI ACTIVE
    fgets(linie, sizeof(linie), f);
    // Inițializăm toate rezervările ca inactive
    for (int i = 0; i < MAX_CAMERE; i++)
    {
        rezervari[i].activa = 0;
    }
    // Citim rezervările active
    while (fgets(linie, sizeof(linie), f))
        {
        if (strstr(linie, "(Nu exista rezervari") != NULL)
            break;
        char tip[20];
        int zile, nr_persoane, ora;
        float pret_total;
        if (sscanf(linie, "%s %d %d %f %d:%*d", tip, &zile, &nr_persoane, &pret_total, &ora) == 5) {
            // Găsim indexul camerei corespunzătoare acestui tip
            for (int i = 0; i < *nr_camere; i++) {
                if (strcmp(camere[i].tip, tip) == 0 && camere[i].ora_disponibilitate == ora) {
                    strcpy(rezervari[i].tip, tip);
                    rezervari[i].zile = zile;
                    rezervari[i].nr_persoane = nr_persoane;
                    rezervari[i].pret_total = pret_total;
                    rezervari[i].activa = 1;
                    break;
                }
            }
        }
    }
    fclose(f);
}
// Funcția principală
int main()
{
    Camera camere[MAX_CAMERE]; //structura de maxim 5 camere
    int nr_camere = 0;
    int optiune, suboptiune;
    int autentificare;
    char tip[20] = "";
    citeste_date(camere, &nr_camere, rezervari);
     //Dacă nu s-au citit date, se inițializează datele implicite
     if (nr_camere == 0) //daca nu am nicio camera atunci se initializeaza datele implicit, in fisier scriindu-se aceste camere
        {
        camere[0] = (Camera){"Standard", 2, 1, 1, 100.50, 14};
        camere[1] = (Camera){"Luxe", 3, 1, 0, 200.75, 16};
        camere[2] = (Camera){"Deluxe", 1, 0, 1, 150.30, 15};
        camere[3] = (Camera){"Economy", 2, 1, 1, 80.00, 12};
        camere[4] = (Camera){"Superior", 4, 1, 0, 250.00, 17};
        nr_camere = 5;
        //for (int i = 0; i < MAX_CAMERE; i++) {
            //rezervari[i].activa = 0;
        //}
        salveaza_date(camere, nr_camere, rezervari); //salvam datele in fisier adica camerele
        printf("Datele initiale au fost salvate.\n");
        }
    autentificare = autentificare_admin();
    clear_screen();
    while (1)
        {
        if (autentificare) //daca adminul a reusit sa se autentifice
        {
            clear_screen();
            printf("\nMeniu " RED "Admin" RESET ":\n");
            printf(RED     "1" RESET ". Modifica pretul unei camere\n");
            printf(GREEN   "2" RESET ". Modifica facilitati pentru o camera\n");
            printf(YELLOW  "3" RESET ". Sterge o camera\n");
            printf(BLUE    "4" RESET ". Afiseaza toate camerele\n");
            printf(MAGENTA "5" RESET ". Iesire in logare\n");
            printf(CYAN    "6" RESET ". Iesire din program\n");
            printf(ORANGE "7" RESET ". Adauga o camera noua\n");
            char buffer[100];
            printf("Alege o optiune (Te rugam sa introduci o cifra intre 1 si 7): ");
            fgets(buffer, sizeof(buffer), stdin); //citim optiunea intr-un buffer
            buffer[strcspn(buffer, "\n")] = 0; //eliminam
            int valid = 1;
            for (int i = 0; i < strlen(buffer); i++)
                {
                if (!isdigit(buffer[i])) //daca nu e cifra ceea ce am introdus
                {
                    valid = 0;
                    break;
                }
            }
        if (!valid || sscanf(buffer, "%d", &optiune) != 1 || optiune < 1 || optiune > 7)
            {
            printf(RED "Optiune invalida" RESET "! Te rugam sa introduci un numar intre 1 si 7.\n");
            printf("Apasa Enter pentru a continua...");
            getchar();
            continue;
           }
            if (optiune == 1)
                {
            char buffer[100];
            int valid = 0;
            while (!valid)
                {
                printf("Introduceti indexul camerei pentru modificare (0 - %d): ", nr_camere - 1);
                fgets(buffer, sizeof(buffer), stdin);
                buffer[strcspn(buffer, "\n")] = 0;

                int cifre = 1;
                for (int i = 0; buffer[i]; i++) {
                    if (!isdigit(buffer[i])) {
                        cifre = 0;
                        break;
                    }
                }

                if (cifre && sscanf(buffer, "%d", &suboptiune) == 1 &&
                    suboptiune >= 0 && suboptiune < nr_camere) {
                    valid = 1;
                } else {
                    printf(RED "Index invalid" RESET "! Introduceti un numar intre 0 si %d.\n", nr_camere - 1);
                }
            }
            modifica_pret(camere, nr_camere, suboptiune);
            }
             else if (optiune == 2) {
                //printf("Introduceti indexul camerei pentru modificare: ");
                if (suboptiune >= 0 && suboptiune < nr_camere) {
                    modifica_facilitati(camere, nr_camere, suboptiune);
                } else {
                    printf("Atentie: Optiune invalida.\n");
                }
            } else if (optiune == 3) {
                sterge_camera(camere, &nr_camere);
            } else if (optiune == 4) {
                afiseaza_camere(camere, nr_camere);
            } else if (optiune == 5) {
                printf("Iesire la logare.\n");
                clear_screen();
                autentificare = autentificare_admin();
            } else if (optiune == 6) {
                printf("Iesire din program.\n");
                exit(0);}
                else if (optiune == 7) {
    adauga_camera(camere, &nr_camere, rezervari);
}
             else {
                printf("Atentie: Optiune invalida. Incercati din nou.\n");
                continue;
            }
        } else {
            clear_screen();
            printf("\nMeniu " GREEN "Client" RESET ":\n");
            printf(RED     "1" RESET ". Cauta dupa tipul camerei\n");
            printf(GREEN   "2" RESET ". Afiseaza toate camerele\n");
            printf(YELLOW  "3" RESET ". Iesire in logare\n");
            printf(BLUE    "4" RESET ". Iesire din program\n");
            printf(MAGENTA "5" RESET ". Rezerva o camera\n");
            printf(CYAN    "6" RESET ". Anuleaza o rezervare\n");
            printf(ORANGE  "7" RESET ". Afiseaza rezervarile active\n");
            char buffer[100];
            printf("Alege o optiune (Te rugam sa introduci o cifra intre 1 si 7): ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            int valid = 1;
            for (int i = 0; i < strlen(buffer); i++)
                {
                if (!isdigit(buffer[i]))
                {
                    valid = 0;
                    break;
                }
               }
            if (!valid || sscanf(buffer, "%d", &optiune) != 1 || optiune < 1 || optiune > 7) {
                printf(RED "Optiune invalida" RESET "! Te rugam sa introduci un numar intre 1 si 6.\n");
                printf("Apasa Enter pentru a continua...");
                getchar();
                continue;
            }
          if (optiune == 1)
            {
        while (1) {
            clear_screen();
            printf("\033[1;33mSubmeniu\033[0m Cauta dupa tipul camerei:\n");
            printf("\033[1;36m1\033[0m. Standard\n");
            printf("\033[1;36m2\033[0m. Luxe\n");
            printf("\033[1;36m3\033[0m. Deluxe\n");
            printf("\033[1;36m4\033[0m. Economy\n");
            printf("\033[1;36m5\033[0m. Superior\n");
            printf("Alege o optiune (1 - 5): ");
            char buffer_tip[100];
            fgets(buffer_tip, sizeof(buffer_tip), stdin);
            buffer_tip[strcspn(buffer_tip, "\n")] = 0;
            if (strlen(buffer_tip) == 1 && isdigit(buffer_tip[0])) {
                suboptiune = buffer_tip[0] - '0';
                if (suboptiune >= 1 && suboptiune <= 5) {
                    if (suboptiune == 1) strcpy(tip, "Standard");
                    else if (suboptiune == 2) strcpy(tip, "Luxe");
                    else if (suboptiune == 3) strcpy(tip, "Deluxe");
                    else if (suboptiune == 4) strcpy(tip, "Economy");
                    else if (suboptiune == 5) strcpy(tip, "Superior");
                    cautare_dupa_tip(camere, nr_camere, tip);
                    printf("\nApasa Enter pentru a continua...");
                    getchar();
                    break;
                }
            }
            printf(RED "Optiune invalida!" RESET " Introduceti un numar intre 1 si 5.\n");
            printf("Apasa Enter pentru a continua...");
            getchar();
        }
            } else if (optiune == 2) {
                afiseaza_camere(camere, nr_camere);
            } else if (optiune == 3) {
                printf("Iesire la logare.\n");
                clear_screen();
                autentificare = autentificare_admin();
            } else if (optiune == 4) {
                printf("Iesire din program.\n");
                exit(0);
            } else if (optiune == 5) {
                rezerva_camera(camere, nr_camere);
            } else if (optiune == 6) {
                anuleaza_rezervare(camere, nr_camere, rezervari);
            }
            else if (optiune == 7) {
    afiseaza_rezervari(camere, nr_camere, rezervari);
       }else {
                printf("Atentie: Optiune invalida. Incercati din nou.\n");
                continue;
            }
        }
    }
    return 0;
}
