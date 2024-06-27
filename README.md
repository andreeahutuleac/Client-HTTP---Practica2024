Proiectul este o aplicatie pentru obtinerea informatiilor meteo utilizand API-ul OpenWeatherMap. Aplicatia este structurata in jurul clasei Weather, care gestioneaza cererile catre API, analizand raspunsurile JSON si XML primite pentru a extrage si a afisa datele meteorologice curente si previziunile pentru locatia specificata.

Structura claselor si functionalitati:

1. Clasa HttpClient

Clasa HttpClient este responsabila pentru gestionarea cererilor HTTP catre API-uri. Foloseste biblioteca libcurl pentru a efectua cereri de tip GET si POST catre serverul OpenWeatherMap. Metodele principale includ request pentru a efectua cereri HTTP, parseJsonResponse pentru analizarea raspunsurilor JSON si parseXmlResponse pentru analizarea raspunsurilor XML. In plus, clasa suporta setari precum timeout-uri, headere HTTP personalizate, gestionarea cookie-urilor, configurarea proxy-urilor si cache-urilor. Utilizeaza si functii callback pentru gestionarea datelor primite de la server si pentru headerele raspunsului.

2. Clasa HttpOptions

Clasa HttpOptions este utilizata pentru a gestiona optiuni specifice pentru cererile HTTP in cadrul proiectului tau, permitand configurarea detaliata a setarilor necesare pentru fiecare cerere.

Membri ai clasei HttpOptions
baseUri: Atribut pentru stocarea URI-ului de baza al serverului catre care se fac cererile HTTP.

headers: Un std::map care contine antetele HTTP personalizate pentru cererea HTTP curenta. Antetele sunt stocate sub forma de perechi cheie-valoare, permitand specificarea si configurarea flexibila a antetelor necesare.

username si password: Atribute pentru stocarea numelui de utilizator si a parolei, utilizate pentru autentificarea in cererile HTTP, daca este necesar. Acestea sunt setate si accesate folosind metodele setUsername, getUsername, setPassword si getPassword.

Metode publice
setBaseUri: Permite setarea URI-ului de baza pentru cererile HTTP. Acesta este adesea utilizat pentru a specifica serverul destinatar al cererii.

setHeader: Permite adaugarea sau actualizarea unui antet HTTP specificat prin headerName si headerValue. Aceste antete sunt adaugate in map-ul headers.

getBaseUri si getHeaders: Metode pentru accesarea URI-ului de baza si a tuturor antetelor configurate.

hasHeader, getHeader si removeHeader: Metode utile pentru verificarea existentei unui antet, obtinerea valorii unui antet sau eliminarea unui antet specificat.

setUsername, getUsername, setPassword, getPassword: Metode pentru setarea si accesarea numelui de utilizator si a parolei utilizate pentru autentificare in cererile HTTP.

hasUsernameAndPassword: Metoda pentru a verifica daca sunt setate numele de utilizator si parola.

3. Clasa Weather

Clasa Weather gestioneaza datele meteo pentru o anumita locatie, identificata prin cityId. Aceasta utilizeaza clasa HttpClient pentru a efectua cereri catre API-ul OpenWeatherMap pentru a obtine date meteo actuale, inclusiv temperatura, umiditatea, viteza vantului, presiunea atmosferica etc. Metodele includ getWeatherData pentru a obtine datele meteorologice curente, start si stop pentru pornirea si oprirea unui fir de executie care actualizeaza periodic datele meteo, parseJsonResponse pentru analizarea raspunsurilor JSON primite de la API, parseXmlResponse pentru analizarea raspunsurilor XML (daca este necesar), setCityId pentru a schimba locatia bazata pe id-ul orasului, si alte metode auxiliare pentru manipularea si citirea datelor din fisiere JSON sau TXT.

Utilizarea in proiect
Clasa HttpOptions este utilizata in cadrul proiectului tau pentru a configura si personaliza cererile HTTP catre API-ul OpenWeatherMap. De exemplu, aceasta este utilizata pentru a seta URI-ul de baza al serverului, pentru a adauga antetele necesare (cum ar fi Content-Type, Authorization etc.) si pentru a gestiona autentificarea, daca este necesara.

Aceste functionalitati ajuta la flexibilitatea si configurabilitatea cererilor HTTP in cadrul aplicatiei tale, permitand o integrare mai usoara si o adaptabilitate crescuta in functie de nevoile si cerintele specifice ale API-ului utilizat.

Prin integrarea corecta a clasei HttpOptions cu clasa HttpClient si alte componente ale proiectului, se asigura o gestionare robusta a cererilor HTTP, manipularea adecvata a datelor si asigurarea securitatii si performantei in comunicarea cu serverele API-ului OpenWeatherMap.

Dependente si librarii utilizate:
libcurl: Pentru gestionarea cererilor HTTP catre API-ul OpenWeatherMap.
JsonCpp: Pentru analiza si manipularea raspunsurilor JSON.
tinyxml2: Pentru analiza si manipularea raspunsurilor XML (optional, in functie de formatul raspunsului primit de la API).