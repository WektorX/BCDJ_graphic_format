#include <exception>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <bitset>
using namespace std;

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;

#define szerokosc 512
#define wysokosc 340

#define tytul "GKiM2020 - Projekt - Baran Mikolaj, Czuba Lukasz, Dyrek Kacper, Jurek Wiktor"


void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel (int x, int y);

void czyscEkran(Uint8 R, Uint8 G, Uint8 B);

void encodeHeader(int w, int h, int p, bool d, bool c);
void decodeHeader();
void defineProperties();

SDL_Color paleta[256];
int ileKolorow = 0;

int dodajKolor(SDL_Color kolor){
    int aktualnyKolor = ileKolorow;
    paleta[aktualnyKolor] = kolor;

    cout<<aktualnyKolor<<": ["<<(int)kolor.r<<", "<<(int)kolor.g<<", "<<(int)kolor.b<<"]"<<endl;

    ileKolorow++;
    return aktualnyKolor;
}

bool porownajKolory(SDL_Color k1, SDL_Color k2){
    bool wynik = true;

    if(k1.r != k2.r) wynik = false;
    if(k1.g != k2.g) wynik = false;
    if(k1.b != k2.b) wynik = false;

    return wynik;
}

int sprawdzKolor(SDL_Color kolor){
    int wynik = -1;

    if(ileKolorow > 0){
        for(int k=0; k<ileKolorow; k++){
            if(porownajKolory(kolor, paleta[k])){
                wynik = k;
            }
        }
    }

    if(wynik < 0){
        wynik = dodajKolor(kolor);
    }

    return wynik;
}

void defineProperties(){
    int width = 512;
    int height = 340;
    int palette;
    bool dithering;
    bool compression = false;
    cout << "Podaj szerokosc bitmapy (max 1024): ";
    cin >> width;
    while(width < 0 || width > 1024){
        cout << "Niepoprawna wartosc. Podaj szerokosc bitmapy jeszcze raz (max 1024): ";
        cin >> width;
    }

    cout << "Podaj wysokosc bitmapy (max 1024): ";
    cin >> height;
    while(height < 0 || height > 1024){
        cout << "Niepoprawna wartosc. Podaj wysokosc bitmapy jeszcze raz (max 1024): ";
        cin >> height;
    }
    cout << "Wybierz rodzaj palety uzywanej do zapisu bitmapy: \n\t 0 - dla palety wbudowanej \n\t 1 - dla palety zlozonej z odcieni szarosci \n\t 2 - dla palety dedykowanej" << endl << "Twoj wybor: ";
    cin >> palette;
    while(palette < 0 || palette > 2){
        cout << "Niepoprawna wartosc. Wybierz rodzaj palety jeszcze raz." << endl;
        cout << "\n\t 0 - dla palety wbudowanej \n\t 1 - dla palety zlozonej z odcieni szarosci \n\t 2 - dla palety dedykowanej" << endl << "Twoj wybor: ";
        cin >> palette;
    }
    cout << "Wybierz, czy do zapisu obrazka uzyc ditheringu: \n\t 0 - nie \n\t 1 - tak" << endl << "Twoj wybor: ";
    cin >> dithering;

    cout << "Konwertuje..." << endl;

    encodeHeader(width, height, palette, dithering, compression);

    cout << "Gotowe" << endl;
}

void encodeHeader(int w, int h, int p, bool d, bool c) {
    SDL_Color kolor;
    char identyfikator[] = "BCDJ";
    Uint8 indeks = 0;
    ileKolorow = 0;

    Uint16 head1 = (Uint16)w;
    cout << "Width: " << bitset<16>(head1) << endl;
    head1 = head1<<6;
    cout << "Width po przesunieciu z samym width: " << bitset<16>(head1) << endl;
    Uint16 temp1 = (Uint16)h;
    cout << "Height: " << bitset<16>(temp1) << endl;
    temp1 = temp1>>4;
    cout << "6 bitow z height: " << bitset<16>(temp1) << endl;
    head1 = head1 | temp1;
    cout << "Width i 6 bitow height: " << bitset<16>(head1) << endl;
    temp1 = (Uint16)h;
    temp1 = temp1<<4;
    cout << "height przesuniety o 4 bity, z ktorego bierzemy 4 bity: " << bitset<16>(temp1) << endl;
    Uint8 head2 = temp1;
    cout << "4 bity height w uint8 head2: " << bitset<8>(temp1) << endl;
    Uint8 temp2 = (Uint8)p;
    temp2 = temp2<<2;
    head2 = head2 | temp2;
    cout << "4 bity height i 2 bity palety: " << bitset<8>(head2) << endl;
    temp2 = (Uint8)d;
    temp2 = temp2<<1;
    head2 = head2 | temp2;
    cout << "4 bity height i 2 bity palety i bit dithering: " << bitset<8>(head2) << endl;
    temp2 = (Uint8)c;
    head2 = head2 | temp2;
    cout << "4 bity height i 2 bity palety, bit dithering i 0 z kompresji: " << bitset<8>(head2) << endl;

    cout << "\n\nPelny naglowek (tylko bez palety): " << bitset<16>(head1) << bitset<8>(head2) << endl;

    cout<<"Zapisujemy plik 'test.bcdj' uzywajac metody write()" << endl;
    ofstream wyjscie("test.bcdj", ios::binary);

    wyjscie.write((char*)&identyfikator, sizeof(char)*4);
    wyjscie.write((char*)&head1, sizeof(Uint16));
    wyjscie.write((char*)&head2, sizeof(Uint8));

    if(p == 2){ // gdy wybrana paleta dedykowana, to ja tworzymy i zapisujemy do naglowka
     /*   for(int y=0; y<h; y++){
            for(int x=0; x<w; x++){
                kolor = getPixel(x,y);
                sprawdzKolor(kolor);
            }
        }

        Uint8 kolory = ileKolorow;
        SDL_Color* paleta_kolorow = new SDL_Color[kolory];

        wyjscie.write((char*)&kolory, sizeof(Uint8));

        for(Uint8 i=0; i<kolory; i++){
            paleta_kolorow[i] = paleta[i];
            kolor = paleta_kolorow[i];
            wyjscie.write((char*)&kolor.r, sizeof(Uint8));
            wyjscie.write((char*)&kolor.g, sizeof(Uint8));
            wyjscie.write((char*)&kolor.b, sizeof(Uint8));
        }

        for(int y=0; y<h; y++){
            for(int x=0; x<w; x++){
                kolor = getPixel(x,y);
                indeks = sprawdzKolor(kolor);
                wyjscie.write((char*)&indeks, sizeof(Uint8));
            }
        }
        */
    }

    wyjscie.close();

    cout << "\n\nTeraz nastapi odczyt przekonwertowanego pliku..." << endl;
    decodeHeader();

    //SDL_UpdateWindowSurface(window);
}

void decodeHeader(){
    char identyfikator[] = "    ";
    Uint16 szerokoscObrazka = 0;
    Uint16 wysokoscObrazka = 0;
    Uint16 head1 = 0;
    Uint8 head2 = 0;
    Uint8 temp1 = 0;
    Uint8 temp2 = 0;
    Uint8 rodzajPalety = 0;
    bool dithering = true;
    bool kompresja = true;

    cout<<"Odczytujemy plik 'test.bcdj' uzywajac metody read()" << endl;
    ifstream wejscie("test.bcdj", ios::binary);

    wejscie.read((char*)&identyfikator, sizeof(char)*4);
    wejscie.read((char*)&head1, sizeof(Uint16));
    szerokoscObrazka = head1>>6;
    wejscie.read((char*)&head2, sizeof(Uint8));
    head1 = head1<<10;
    wysokoscObrazka = (head1>>6) | (head2>>4);
    temp1 = head2<<4;
    rodzajPalety = temp1>>6;
    temp2 = temp1<<2;
    dithering = temp2>>7;
    temp2 = temp2<<1;
    kompresja = temp2>>7;

    cout << "id: " << identyfikator<< endl;
    cout << "szerokosc: " << szerokoscObrazka << endl;
    cout << "wysokosc: " << wysokoscObrazka << endl;
    cout << "rodzaj palety: " << (int)rodzajPalety << endl;
    cout << "dithering: " << dithering << endl;
    cout << "kompresja: " << kompresja << endl;

    wejscie.close();
}

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc))
  {
    /* Zamieniamy poszczególne składowe koloru na format koloru piksela */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* Pobieramy informację ile bajtów zajmuje jeden piksel */
    int bpp = screen->format->BytesPerPixel;

    /* Obliczamy adres piksela */
    Uint8 *p1 = (Uint8 *)screen->pixels + (y*2) * screen->pitch + (x*2) * bpp;
    Uint8 *p2 = (Uint8 *)screen->pixels + (y*2+1) * screen->pitch + (x*2) * bpp;
    Uint8 *p3 = (Uint8 *)screen->pixels + (y*2) * screen->pitch + (x*2+1) * bpp;
    Uint8 *p4 = (Uint8 *)screen->pixels + (y*2+1) * screen->pitch + (x*2+1) * bpp;

    /* Ustawiamy wartość piksela, w zależnoœci od formatu powierzchni*/
    switch(bpp)
    {
        case 1: //8-bit
            *p1 = pixel;
            *p2 = pixel;
            *p3 = pixel;
            *p4 = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p1 = pixel;
            *(Uint16 *)p2 = pixel;
            *(Uint16 *)p3 = pixel;
            *(Uint16 *)p4 = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p1[0] = (pixel >> 16) & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = pixel & 0xff;
                p2[0] = (pixel >> 16) & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = pixel & 0xff;
                p3[0] = (pixel >> 16) & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = pixel & 0xff;
                p4[0] = (pixel >> 16) & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = pixel & 0xff;
            } else {
                p1[0] = pixel & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = (pixel >> 16) & 0xff;
                p2[0] = pixel & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = (pixel >> 16) & 0xff;
                p3[0] = pixel & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = (pixel >> 16) & 0xff;
                p4[0] = pixel & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p1 = pixel;
            *(Uint32 *)p2 = pixel;
            *(Uint32 *)p3 = pixel;
            *(Uint32 *)p4 = pixel;
            break;

        }
    }
}

void setPixelSurface(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<szerokosc*2) && (y>=0) && (y<wysokosc*2))
  {
    /* Zamieniamy poszczególne składowe koloru na format koloru piksela */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* Pobieramy informację ile bajtów zajmuje jeden piksel */
    int bpp = screen->format->BytesPerPixel;

    /* Obliczamy adres piksela */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    /* Ustawiamy wartość piksela, w zależności od formatu powierzchni*/
    switch(bpp)
    {
        case 1: //8-bit
            *p = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p = pixel;
            break;
        }
    }
}

SDL_Color getPixel(int x, int y) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc)) {
        //określamy pozycję
        char* pPosition=(char*)screen->pixels ;

        //przesunięcie względem y
        pPosition+=(screen->pitch*y*2) ;

        //przesunięcie względem x
        pPosition+=(screen->format->BytesPerPixel*x*2);

        //kopiujemy dane piksela
        memcpy(&col, pPosition, screen->format->BytesPerPixel);

        //konwertujemy kolor
        SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}

SDL_Color getPixelSurface(int x, int y, SDL_Surface *surface) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc)) {
        //określamy pozycję
        char* pPosition=(char*)surface->pixels ;

        //przesunięcie względem y
        pPosition+=(surface->pitch*y) ;

        //przesunięcie względem x
        pPosition+=(surface->format->BytesPerPixel*x);

        //kopiujemy dane piksela
        memcpy(&col, pPosition, surface->format->BytesPerPixel);

        //konwertujemy kolor
        SDL_GetRGB(col, surface->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}


void ladujBMP(char const* nazwa, int x, int y)
{
    SDL_Surface* bmp = SDL_LoadBMP(nazwa);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
    }
    else
    {
        SDL_Color kolor;
        for (int yy=0; yy<bmp->h; yy++) {
			for (int xx=0; xx<bmp->w; xx++) {
				kolor = getPixelSurface(xx, yy, bmp);
				setPixel(xx, yy, kolor.r, kolor.g, kolor.b);
			}
        }
		SDL_FreeSurface(bmp);
        SDL_UpdateWindowSurface(window);
    }

}


void czyscEkran(Uint8 R, Uint8 G, Uint8 B)
{
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, R, G, B));
    SDL_UpdateWindowSurface(window);
}



int main(int argc, char* argv[]) {
    int choice = 0;

    do{
        cout << "Wybierz, co chcesz zrobic: \n\t1 - dla konwersji bitmapy do autorskiego rozszerzenia \n\t2 - dla konwersji pliku z autorskim rozszerzeniem do bitmapy \n\t0 - dla wyjscia z programu" << endl << "Twoj wybor: ";
        cin >> choice;
        while(choice < 0 || choice > 2){
            cout << "Niepoprawny wybor. Wybierz jeszcze raz." << endl;
            cout << "Wybierz, co chcesz zrobic: \n\t1 - dla konwersji bitmapy do autorskiego rozszerzenia \n\t2 - dla konwersji pliku z autorskim rozszerzeniem do bitmapy \n\t0 - dla wyjscia z programu" << endl << "Twoj wybor: ";
            cin >> choice;
        }

        switch(choice){
        case 0:
            break;
        case 1:
            defineProperties();
            break;
        case 2:
            decodeHeader();
            break;
        default:
            break;
        }
    }while(choice != 0);


    /*if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(tytul, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szerokosc*2, wysokosc*2, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    screen = SDL_GetWindowSurface(window);
    if (screen == NULL) {
        fprintf(stderr, "SDL_GetWindowSurface Error: %s\n", SDL_GetError());
    return false;
    }
    SDL_UpdateWindowSurface(window);


    bool done = false;
    SDL_Event event;
    // główna pętla programu
    while (SDL_WaitEvent(&event)) {
        // sprawdzamy czy pojawiło się zdarzenie
        switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;
            // sprawdzamy czy został wciśnięty klawisz
            case SDL_KEYDOWN: {
                // wychodzimy, gdy wciśnięto ESC
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    done = true;
                if (event.key.keysym.sym == SDLK_a)
                    ladujBMP("obrazek1.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_s)
                    ladujBMP("obrazek2.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_d)
                    ladujBMP("obrazek3.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_f)
                    ladujBMP("obrazek4.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_g)
                    ladujBMP("obrazek5.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_h)
                    ladujBMP("obrazek6.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_b)
                    czyscEkran(0, 0, 10);
                else
                    break;
               }
        }
        if (done) break;
    }

    if (screen) {
        SDL_FreeSurface(screen);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }


    SDL_Quit();*/
    return 0;
}
