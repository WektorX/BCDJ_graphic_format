#include <exception>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <bitset>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;

#define tytul "GKiM2020 - Projekt - Baran Mikolaj, Czuba Lukasz, Dyrek Kacper, Jurek Wiktor"

int szerokosc, wysokosc;
void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixelSurface(int x, int y, SDL_Surface *surface);
void ladujBMP(char const* nazwa, int x, int y);
SDL_Color getPixel (int x, int y);
void czyscEkran(Uint8 R, Uint8 G, Uint8 B);

void encodeHeader(string name, int w, int h, int p, bool d, bool c, SDL_Surface* bmp);
void decodeHeader();
void defineProperties();
int preInspection(int w, int h, char* name);
int preInspectionDithering(int w, int h, char* name,int palette);

struct colorPalette
{
    SDL_Color color;
    int counter = 0;
};
bool sortByCounter(const colorPalette &lhs, const colorPalette &rhs)
{
    return lhs.counter > rhs.counter;
}

vector<colorPalette> customPalette;
SDL_Color finalCustomPalette[16];
vector<vector<colorPalette>> listOfPalettes;

SDL_Color greyPalette[16];
SDL_Color preDefPalette[16];

char findWidestRangeColor();
void generateCustomPalette();
void customPaletteFunction();
void replacePixelsWithCustomPaletteColors();

SDL_Color pixelToPreDefPalette(SDL_Color color)
{
    int R,G,B;
    R = (int)color.r;
    G = (int)color.g;
    B = (int)color.b;

    R = round(R * 1.0 /255);
    G = round(G * 3.0 / 255);
    B = round(B * 1.0 / 255);

    R = R * 255;
    G = (G * 255) / 3.0;
    B = B *255;

    color.r = R;
    color.g = G;
    color.b = B;

    return color;
}

void preDefPaletteFunction()
{
    int R,G,B;
    SDL_Color color;
    for(int y=0; y<wysokosc; y++)
    {
        for(int x=0; x<szerokosc; x++)
        {
            color = getPixel(x,y);
            color = pixelToPreDefPalette(color);

            R = (int)color.r;
            G = (int)color.g;
            B = (int)color.b;

            setPixel(x,y,R,G,B);
        }
    }
}

int pixelToGreayPalette(SDL_Color color)
{
    int R,G,B, BW;

    R = (int)color.r;
    G = (int)color.g;
    B = (int)color.b;

    BW = round(0.299 * R + 0.587 * G + 0.114 *B);
    BW = round(BW * 15.0 / 255);
    BW = BW * 255 / 15.0;

    return BW;
}

void greyPaletteFunction()
{
    SDL_Color color;
    int BW;
    for(int y=0; y<wysokosc; y++)
    {
        for(int x=0; x<szerokosc; x++)
        {
            color = getPixel(x,y);
            BW = pixelToGreayPalette(color);
            setPixel(x,y,BW,BW,BW);
        }
    }

}

void addErrToRGB(int c, int err, SDL_Color* bmp_map) {
    if(c == 0) {
        if( bmp_map->r + err > 255 ) bmp_map->r = 255;
        else if( bmp_map->r + err < 0 ) bmp_map->r = 0;
        else bmp_map->r += err;
    }
    else if(c == 1) {
        if( bmp_map->g + err > 255 ) bmp_map->g = 255;
        else if( bmp_map->g + err < 0 ) bmp_map->g = 0;
        else bmp_map->g += err;
    }
    else if(c == 2) {
        if( bmp_map->b + err > 255 ) bmp_map->b = 255;
        else if( bmp_map->b + err < 0 ) bmp_map->b = 0;
        else bmp_map->b += err;
    }
}

void Dithering(int palette, char* name)
{
    int err_R, err_G, err_B;
    SDL_Color color, paletteColor;

    SDL_Surface* bmp = SDL_LoadBMP(name);
    SDL_Color bmp_map[bmp->w][bmp->h];

    for(int y=0; y<bmp->h; y++) {
        for(int x=0; x<bmp->w; x++) {
            if(palette == 1) {
                int R,G,B, BW;
                color = getPixelSurface(x, y, bmp);

                R = (int)color.r;
                G = (int)color.g;
                B = (int)color.b;

                BW = round(0.299 * R + 0.587 * G + 0.114 *B);

                bmp_map[x][y].r = BW;
                bmp_map[x][y].g = BW;
                bmp_map[x][y].b = BW;
            }
            else bmp_map[x][y] = getPixelSurface(x, y, bmp);
        }
    }

    for(int y=0; y<wysokosc; y++)
    {
        for(int x=0; x<szerokosc; x++)
        {
            color = bmp_map[x][y];

            if(palette == 1) {
                int BW = color.r;
                BW = round(BW * 15.0 / 255);
                BW = BW * 255 / 15.0;
                paletteColor.r = BW;
                paletteColor.g = BW;
                paletteColor.b = BW;
            }
            else if(palette == 0) paletteColor = pixelToPreDefPalette(color);
            else if(palette == 2) paletteColor = pixelToPreDefPalette(color);


            setPixel(x, y, (int)paletteColor.r, (int)paletteColor.g, (int)paletteColor.b);

            err_R = (int)color.r - (int)paletteColor.r;
            err_G = (int)color.g - (int)paletteColor.g;
            err_B = (int)color.b - (int)paletteColor.b;

            if(x < (szerokosc - 1)) {
                addErrToRGB(0, ( err_R * 7/16 ), &bmp_map[x+1][y]);
                addErrToRGB(1, ( err_G * 7/16 ), &bmp_map[x+1][y]);
                addErrToRGB(2, ( err_B * 7/16 ), &bmp_map[x+1][y]);
            }

            if(x > 0 && y < (wysokosc - 1) ) {
                addErrToRGB(0, ( err_R * 3/16 ), &bmp_map[x-1][y+1]);
                addErrToRGB(1, ( err_G * 3/16 ), &bmp_map[x-1][y+1]);
                addErrToRGB(2, ( err_B * 3/16 ), &bmp_map[x-1][y+1]);
            }

            if(y < (wysokosc - 1) ) {
                addErrToRGB(0, ( err_R * 3/16 ), &bmp_map[x][y+1]);
                addErrToRGB(1, ( err_G * 3/16 ), &bmp_map[x][y+1]);
                addErrToRGB(2, ( err_B * 3/16 ), &bmp_map[x][y+1]);
            }

            if(x < (szerokosc - 1) && y < (wysokosc - 1) ) {
                addErrToRGB(0, ( err_R * 3/16 ), &bmp_map[x+1][y+1]);
                addErrToRGB(1, ( err_G * 3/16 ), &bmp_map[x+1][y+1]);
                addErrToRGB(2, ( err_B * 3/16 ), &bmp_map[x+1][y+1]);
            }
        }
    }
    SDL_UpdateWindowSurface(window);
}


void defineProperties()
{
    string name;
    int width = 512;
    int height = 340;
    int palette;
    bool dithering;
    bool compression = false;

    cout << "Podaj nazwe pliku bitmapy: ";
    cin >> name;

    //zamiana string na char;
    char name_char[name.length()+1];
    strcpy(name_char, name.c_str());

    SDL_Surface* bmp = SDL_LoadBMP(name_char);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
    }

    width = bmp->w;
    szerokosc = width;  //przypisanie globalnej szerokosci potrzebnej np do metody setpixel;
    cout << "Szerokosc bitmapy: " << width << endl;

    height = bmp->h;
    wysokosc = height;  //przypisanie globalnej wysokosci potrzebnej np do metody setpixel;
    cout << "Wysokosc bitmapy: " << height << endl;

    if(height > 1024 || width > 1024)
    {
        //nazwa za duzej bitmapy do testow: oversize.bmp
        cout << "Wybierz obraz o mniejszej rozdzielczosci (max: 1024/1024px)!" << endl << endl;
    }
    else
    {
        cout << "Wybierz rodzaj palety uzywanej do zapisu bitmapy: \n\t 0 - dla palety wbudowanej \n\t 1 - dla palety zlozonej z odcieni szarosci \n\t 2 - dla palety dedykowanej";
        // generujemy customowa palete, aby byla dostepna do podgladu //
        customPaletteFunction();
        palette = preInspection(width, height, name_char);
        cout << "Twoj wybor: "<<palette<<endl;
        while(palette < 0 || palette > 2)
        {
            cout << "Niepoprawna wartosc. Wybierz rodzaj palety jeszcze raz." << endl;
            cout << "\n\t 0 - dla palety wbudowanej \n\t 1 - dla palety zlozonej z odcieni szarosci \n\t 2 - dla palety dedykowanej" << endl << "Twoj wybor: ";
            cin >> palette;
        }
        cout << "Wybierz, czy do zapisu obrazka uzyc ditheringu: \n\t 0 - nie \n\t 1 - tak" << endl;
        dithering = preInspectionDithering(width, height, name_char, palette);
        cout<< "Twoj wybor: "<<dithering<<endl;

        cout << endl << "Konwertuje..." << endl << endl;

        encodeHeader(name, width, height, palette, dithering, compression, bmp);
    }
};


int widestColor = 0;
int redL = 255, greenL = 255, blueL = 255, redH = 0, greenH = 0, blueH = 0;
int redRange = 0, greenRange = 0, blueRange = 0;

char findWidestRangeColor(){

    for(colorPalette &element : customPalette){
        if(element.color.r > redH) redH = element.color.r;
        if(element.color.r < redL) redL = element.color.r;
        if(element.color.g > greenH) greenH = element.color.g;
        if(element.color.g < greenL) greenL = element.color.g;
        if(element.color.b > blueH) blueH = element.color.b;
        if(element.color.b < blueL) blueL = element.color.b;
    }

    redRange = redH - redL;
    greenRange = greenH - greenL;
    blueRange = blueH - blueL;

    widestColor = max(max(redRange, greenRange), blueRange);
    if(widestColor == redRange){
        return 'r';
    }
    else if(widestColor == greenRange){
        return 'g';
    }
    else{
        return 'b';
    }
}

void medianCut(){
    int colorRange = 0, colorL = 0, colorH = 0;
    char channel = findWidestRangeColor();

    switch(channel){
        case 'r':
            sort(customPalette.begin(), customPalette.end(),[](const colorPalette &lhs, const colorPalette &rhs){return lhs.color.r < rhs.color.r;});
            colorRange = redRange;
            colorL = redL;
            colorH = redH;
            break;
        case 'g':
            sort(customPalette.begin(), customPalette.end(),[](const colorPalette &lhs, const colorPalette &rhs){return lhs.color.g < rhs.color.g;});
            colorRange = greenRange;
            colorL = greenL;
            colorH = greenH;
            break;
        case 'b':
            sort(customPalette.begin(), customPalette.end(),[](const colorPalette &lhs, const colorPalette &rhs){return lhs.color.b < rhs.color.b;});
            colorRange = blueRange;
            colorL = blueL;
            colorH = blueH;
            break;
        default:
            break;
    }

    vector<vector<colorPalette>> tmpListOfPalettes;
    listOfPalettes.push_back(customPalette);
    int divisions = 1;
    vector<colorPalette> tmpPalette1;
    vector<colorPalette> tmpPalette2;
    float median = 0;
    colorPalette tmpColor;

    while(listOfPalettes.size() < 16){
        for(int i=0; i<divisions; i++){
            tmpPalette1.clear();
            tmpPalette2.clear();
            switch(channel){
                case 'r':
                    median = (listOfPalettes[i][listOfPalettes[i].size()-1].color.r - listOfPalettes[i][0].color.r)/2.0;
                     break;
                case 'g':
                    median = (listOfPalettes[i][listOfPalettes[i].size()-1].color.g - listOfPalettes[i][0].color.g)/2.0;
                    break;
                case 'b':
                    median = (listOfPalettes[i][listOfPalettes[i].size()-1].color.b - listOfPalettes[i][0].color.b)/2.0;
                    break;
                default:
                    cout << "Error" << endl;
                    median = (listOfPalettes[i][listOfPalettes[i].size()-1].color.b - listOfPalettes[i][0].color.b)/2.0;
                    break;
            }
            tmpColor = listOfPalettes[i][0];
            for(const auto &v : listOfPalettes[i]){
                switch(channel){
                    case 'r':
                        if(v.color.r <= median + tmpColor.color.r){
                            tmpPalette1.push_back(v);
                        }
                        else{
                            tmpPalette2.push_back(v);
                        }
                        break;
                    case 'g':
                        if(v.color.g <= median + tmpColor.color.r){
                            tmpPalette1.push_back(v);
                        }
                        else{
                            tmpPalette2.push_back(v);
                        }
                        break;
                    case 'b':
                        if(v.color.b <= median + tmpColor.color.r){
                            tmpPalette1.push_back(v);
                        }
                        else{
                            tmpPalette2.push_back(v);
                        }
                        break;
                    default:
                        break;
                }
            }
            tmpListOfPalettes.push_back(tmpPalette1);
            tmpListOfPalettes.push_back(tmpPalette2);
        }
        listOfPalettes.clear();
        for(int x=0; x<divisions*2; x++){
            listOfPalettes.push_back(tmpListOfPalettes[x]);
        }
        tmpListOfPalettes.clear();
        divisions *= 2;
        cout << "Rozmiar palety customowej: " << listOfPalettes.size() << endl;
    }

    generateCustomPalette();
}

void replacePixelsWithCustomPaletteColors(){
    SDL_Color color;
    bool findColor = false;
    for(int y=0; y<wysokosc; y++){
        for(int x=0; x<szerokosc; x++){
            color = getPixel(x, y);
            while(!findColor){
                for(int j=0; j<listOfPalettes.size(); j++){
                    for(int i=0; i<listOfPalettes[j].size(); i++){
                        if(listOfPalettes[j][i].color.r == color.r &&  listOfPalettes[j][i].color.g == color.g && listOfPalettes[j][i].color.b == color.b){
                            setPixel(x,y,finalCustomPalette[j].r, finalCustomPalette[j].g, finalCustomPalette[j].b);
                            cout << "Ustawiam" << endl;
                            // tu docelowo pewnie zapiszemy dane do tablicy, by mozna bylo je potem skompresowac i wpisac do pliku //
                            findColor = true;
                            break;
                        }
                    }
                    if(findColor) break;
                }
            }
        }
    }
}

void generateCustomPalette(){
    int avgR = 0, avgG = 0, avgB = 0, weightSum = 0;
    SDL_Color finalColor;

    for(int i=0; i<listOfPalettes.size(); i++){
        for(int j=0; j<listOfPalettes[i].size(); j++){
            avgR += listOfPalettes[i][j].color.r * listOfPalettes[i][j].counter;
            avgG += listOfPalettes[i][j].color.g * listOfPalettes[i][j].counter;
            avgB += listOfPalettes[i][j].color.b * listOfPalettes[i][j].counter;
            weightSum += listOfPalettes[i][j].counter;
        }
        avgR /= weightSum;
        avgG /= weightSum;
        avgB /= weightSum;
        finalColor.r = avgR;
        finalColor.g = avgG;
        finalColor.b = avgB;
        finalCustomPalette[i] = finalColor;
        // !kolor wygenerowany z n-tego setu kolorow w listOfPalettes jest na n-tej pozycji w finalCustomPalette!
        avgR = 0;
        avgG = 0;
        avgB = 0;
        weightSum = 0;
    }

    cout << "Customowa paleta: " << endl;
    for(int a=0; a<16; a++){
        cout << a << ": [" << (int)finalCustomPalette[a].r << ", " << (int)finalCustomPalette[a].g << ", " << (int)finalCustomPalette[a].b << "]" << endl;
    }
}

void customPaletteFunction(){
    SDL_Color color;
    customPalette.clear();
    for(int y = 0; y < wysokosc; y++)
    {
        for(int x = 0; x < szerokosc; x++)
        {
            color = getPixel(x, y);

            bool existFlag = false;
            for(colorPalette &element : customPalette)
            {
                if( (int)color.r == element.color.r &&
                    (int)color.g == element.color.g &&
                    (int)color.b == element.color.b )
                {
                    existFlag = true;
                    element.counter++;
                }
            }

            if(!existFlag)
            {
                colorPalette newColor;
                newColor.color = color;
                newColor.counter++;
                customPalette.push_back(newColor);
            }
        }
    }
    colorPalette blankColor;
    blankColor.color.a = 1;
    blankColor.color.r = 0;
    blankColor.color.g = 0;
    blankColor.color.b = 0;
    blankColor.counter = -1;
    if(customPalette.size() <= 16){
        if(customPalette.size() < 16){
            for(int i=0; i<16-customPalette.size(); i++){
                customPalette.push_back(blankColor);
            }
        }

        for(int c=0; c<16; c++){
            finalCustomPalette[c] = customPalette[c].color;
        }
    }
    else{
        medianCut();
    }
}

void encodeHeader(string name, int w, int h, int p, bool d, bool c, SDL_Surface* bmp)
{
    char identyfikator[] = "BCDJ";

    int ileKolorow = 0;
    string newName = name.erase(name.size() - 4) + ".bcdj";

    Uint16 head1 = (Uint16)w;
    //cout << "Width: " << bitset<16>(head1) << endl;
    head1 = head1<<6;
    //cout << "Width po przesunieciu z samym width: " << bitset<16>(head1) << endl;
    Uint16 temp1 = (Uint16)h;
    //cout << "Height: " << bitset<16>(temp1) << endl;
    temp1 = temp1>>4;
    //cout << "6 bitow z height: " << bitset<16>(temp1) << endl;
    head1 = head1 | temp1;
    //cout << "Width i 6 bitow height: " << bitset<16>(head1) << endl;
    temp1 = (Uint16)h;
    temp1 = temp1<<4;
    //cout << "height przesuniety o 4 bity, z ktorego bierzemy 4 bity: " << bitset<16>(temp1) << endl;
    Uint8 head2 = temp1;
    //cout << "4 bity height w uint8 head2: " << bitset<8>(temp1) << endl;
    Uint8 temp2 = (Uint8)p;
    temp2 = temp2<<2;
    head2 = head2 | temp2;
    //cout << "4 bity height i 2 bity palety: " << bitset<8>(head2) << endl;
    temp2 = (Uint8)d;
    temp2 = temp2<<1;
    head2 = head2 | temp2;
    //cout << "4 bity height i 2 bity palety i bit dithering: " << bitset<8>(head2) << endl;
    temp2 = (Uint8)c;
    head2 = head2 | temp2;
    //cout << "4 bity height i 2 bity palety, bit dithering i 0 z kompresji: " << bitset<8>(head2) << endl;

    //cout << "Pelny naglowek (tylko bez palety): " << bitset<16>(head1) << bitset<8>(head2) << endl;

    cout<<"Zapisujemy plik " << newName <<" uzywajac metody write()" << endl << endl;
    ofstream wyjscie(newName, ios::binary);

    wyjscie.write((char*)&identyfikator, sizeof(char)*4);
    wyjscie.write((char*)&head1, sizeof(Uint16));
    wyjscie.write((char*)&head2, sizeof(Uint8));

    if(p == 2)   // gdy wybrana paleta dedykowana, to ja tworzymy i zapisujemy do naglowka
    {
        for(int i = 0; i < 16; i++)
        {
            wyjscie.write((char*)&finalCustomPalette[i].r, sizeof(Uint8));
            wyjscie.write((char*)&finalCustomPalette[i].g, sizeof(Uint8));
            wyjscie.write((char*)&finalCustomPalette[i].b, sizeof(Uint8));
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    wyjscie.close();
}

void decodeHeader()
{
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
    string name;

    cout << "Podaj nazwe pliku do zdekodowania: ";
    cin >> name;

    cout << endl << "Odczytujemy plik " << name << " uzywajac metody read()" << endl;
    ifstream wejscie(name, ios::binary);

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

int preInspection(int w, int h, char* name)
{
    cout << endl << "(za pomoca klawiszy '0','1','2' zmieniaj palety,  Esc - aby przejsc dalej)"<< endl;
    int out=0;
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(tytul, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w*2, h*2, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    screen = SDL_GetWindowSurface(window);
    if (screen == NULL)
    {
        fprintf(stderr, "SDL_GetWindowSurface Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_UpdateWindowSurface(window);
    ladujBMP(name, 0, 0);

    bool done = false;
    SDL_Event event;
    // główna pętla programu
    while (SDL_WaitEvent(&event))
    {
        // sprawdzamy czy pojawiło się zdarzenie
        switch (event.type)
        {
        case SDL_QUIT:
            done = true;
            break;
        // sprawdzamy czy został wciśnięty klawisz
        case SDL_KEYDOWN:
        {
            // wychodzimy, gdy wciśnięto ESC
            if (event.key.keysym.sym == SDLK_ESCAPE)
                done = true;
            if (event.key.keysym.sym == SDLK_0)
            {
                ladujBMP(name, 0, 0);
                preDefPaletteFunction();
                out = 0;
                SDL_UpdateWindowSurface(window);
            }
            if (event.key.keysym.sym == SDLK_1)
            {

                ladujBMP(name, 0, 0);
                greyPaletteFunction();
                out = 1;
                SDL_UpdateWindowSurface(window);

            }
            if (event.key.keysym.sym == SDLK_2)
            {
                ladujBMP(name, 0, 0);
                replacePixelsWithCustomPaletteColors();
                out = 2;
                SDL_UpdateWindowSurface(window);
            }
            else
                break;
        }
        }
        if (done) break;
    }

    /*    if (screen)
        {
            SDL_FreeSurface(screen);
        }

        if (window)
        {
            SDL_DestroyWindow(window);
        }

        SDL_Quit();*/
    return out;
}



int preInspectionDithering(int w, int h, char* name, int palette)
{
    cout << endl << "(za pomoca klawiszy '0','1' podgląd z ditheringiem lub bez,  Esc - aby przejsc dalej)"<< endl;
    int out;
    bool done = false;

    SDL_Event event;
    // główna pętla programu
    while (SDL_WaitEvent(&event))
    {
        // sprawdzamy czy pojawiło się zdarzenie
        switch (event.type)
        {
        case SDL_QUIT:
            done = true;
            break;
        // sprawdzamy czy został wciśnięty klawisz
        case SDL_KEYDOWN:
        {
            // wychodzimy, gdy wciśnięto ESC
            if (event.key.keysym.sym == SDLK_ESCAPE)
                done = true;
            if (event.key.keysym.sym == SDLK_0)
            {
                out = 0;
                ladujBMP(name, 0, 0);
                switch(palette)
                {
                case 0:
                    preDefPaletteFunction();
                    break;
                case 1:
                    greyPaletteFunction();
                    break;
                case 2:
                    replacePixelsWithCustomPaletteColors();
                    break;
                }
                SDL_UpdateWindowSurface(window);
            }
            if (event.key.keysym.sym == SDLK_1)
            {
                out = 1;
                ladujBMP(name, 0, 0);

                Dithering(palette, name);
                //cout<<"dithering"<<endl;
                SDL_UpdateWindowSurface(window);

            }
            else{
                break;
            }
        }
        }
        if (done) break;
    }

    return out;
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
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
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
            }
            else
            {
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
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
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

SDL_Color getPixel(int x, int y)
{
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc))
    {
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

SDL_Color getPixelSurface(int x, int y, SDL_Surface *surface)
{
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc))
    {
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
        for (int yy=0; yy<bmp->h; yy++)
        {
            for (int xx=0; xx<bmp->w; xx++)
            {
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



int main(int argc, char* argv[])
{
    //wypelnienie palet
    for(int i = 0; i < 16; i++)
    {
        greyPalette[i].r = i * 17;
        greyPalette[i].g = i * 17;
        greyPalette[i].b = i * 17;
    }

    for(int i = 0; i < 16; i++)
    {
        Uint8 bin = i;
        int r = bitset<1>(bin>>3).to_ulong() * 255;
        int g = bitset<2>(bin>>1).to_ulong() * 85;
        int b = bitset<1>(bin).to_ulong() * 255;
        preDefPalette[i].r = r;
        preDefPalette[i].g = g;
        preDefPalette[i].b = b;
    }

    //koniec wypelniana palet

    int choice = 0;

    do
    {
        cout << "Wybierz, co chcesz zrobic: \n\t1 - dla konwersji bitmapy do autorskiego rozszerzenia \n\t2 - dla konwersji pliku z autorskim rozszerzeniem do bitmapy \n\t0 - dla wyjscia z programu" << endl << "Twoj wybor: ";
        cin >> choice;
        while(choice < 0 || choice > 2)
        {
            cout << "Niepoprawny wybor. Wybierz jeszcze raz." << endl;
            cout << "Wybierz, co chcesz zrobic: \n\t1 - dla konwersji bitmapy do autorskiego rozszerzenia \n\t2 - dla konwersji pliku z autorskim rozszerzeniem do bitmapy \n\t0 - dla wyjscia z programu" << endl << "Twoj wybor: ";
            cin >> choice;
        }

        switch(choice)
        {
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
    }
    while(choice != 0);

    return 0;
}
