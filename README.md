# SDTEMA1

*Plesa Marian Cosmin 311CA*

1. INIT_HEAP

Această funcție inițializează heap-ul, adică zona de memorie în care vor fi
alocate dinamic blocurile de memorie din segregated free lists, adica cele
libere. Functia primeste ca parametrii, structura sfl ce contine date despre
vectorul de liste si vectorul de liste, adresa de inceput, nr de bytes per
lista si numarul de liste

2. MALLOC

Această funcție este responsabilă pentru alocarea de memorie din heap. Ea
caută în vectorul de liste de blocuri libere un bloc de memorie potrivit
pentru a fi  alocat și îl adaugă la lista blocurilor alocate. Dacă nu se
găsește suficient spațiu liber, afișează un mesaj de eroare "Out of memory".

3. FREE

Această funcție eliberează un bloc de memorie alocat anterior. Ea caută 
blocul cu adresa specificată în lista blocurilor alocate și îl elimină 
din această listă. Apoi, adaugă blocul eliberat la lista blocurilor libere,
încercând să rezolve fragmentarea memoriei.

4. READ

Această funcție citeste date din blocurile de memorie alocate la adresa 
specificată și cu dimensiunea specificată. Dacă adresa specificată nu este
validă sau dacă dimensiunea depășește dimensiunea blocului, afișează un 
mesaj de eroare "Segmentation fault (core dumped)". Si dupa in main se
apeleaza destroy heap.
 
5. WRITE

Această funcție scrie date în blocurile de memorie alocate la adresa 
specificată. Dacă adresa specificată nu este validă sau dacă dimensiunea 
depășește dimensiunea blocului, afișează un mesaj de eroare "Segmentation 
fault (core dumped)". Si la fel in cazul de Segmentation fault se apeleaza
destroy heap.

6. DUMP_MEMORY

Această funcție afișează o imagine de ansamblu a stării memoriei heap. 
Ea calculează și afișează următoarele informații:

    Memoria totală disponibilă în heap.
    Memoria totală alocată din heap.
    Memoria liberă disponibilă în heap.
    Numărul de blocuri libere.
    Numărul de blocuri alocate.
    Numărul total de apeluri malloc.
    Numărul total de fragmentări.
    Numărul total de apeluri free.
    Lista blocurilor libere, grupate pe dimensiuni.
    Lista blocurilor alocate, împreună cu adresele și dimensiunile lor.
    
7. DESTROY_HEAP

Această funcție distruge heap-ul și eliberează toată memoria alocată dinamic.
Ea eliberează întâi toate blocurile de memorie alocate și apoi șterge toate 
listele de blocuri libere. La final, eliberează și memoria alocată pentru 
lista de liste de blocuri libere.
