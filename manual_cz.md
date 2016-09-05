# Úvod

Pisoar \(PIcture SOrter for ARchaeology\) je program, který usnadňuje vytváření strukturovaných obrazových výstupů z fotografií objektů či objektových nákresů.

Pomocí programu je jednoduše možné:

* vytvořit databázi pojmenovaných objektů
* přiřadit objektům různé pohledy získané z obrazových souborů
* složit pohledy objektu do zobrazení v jednotném stylu
* vyskládat a uspořádat zobrazení více objektů na plochu
* zobrazit objekty na ploše v jednotném měřítku nezávisle na rozlišení fotografie
* uložit zaplněné plochy jako obrazový soubor

# Příprava vstupího formátu

V programu je možné použít dva způsoby získání pohledů objektů z fotografií.
Od toho se odvíjí požadovaný formát vstupních fotografií.

1. Označení čtverce

   První způsob je vhodný pro ruční kresby na bílém papíře, kdy je obrázek černobílý.
Lze jej také použít u fotografií objektů zachycených na dobře nasvětleném bílem podkladu.
V programu se jednoduše vybere čtvercová oblast ze vstupní fotografie, která se použije pro jeden pohled objektu.
Pro tento způsob není potřeba fotografie nějak upravovat.

2. Automatické nalezení okraje

   Druhý způsob je vhodný pro fotografie, které nemají pozadí ideálně bílé, nebo když potřebujeme přesně vyříznout nepravidelný tvar.
Objekty na těchto fotografiích je nutné oříznout tak, aby měly kolem sebe několik pixelů čistě bílé barvy.
Podle toho program rozpozná hranice objektu na fotografii.
Je nutné, aby to byla zcela bílá barva, jinou okrajovou barvu program nedokáže zpracovat.

### Doporučené uspořádání fotografií

I když je podporováno jakékoliv uspořádání objektů na fotografiích, pro větší efektivitu práce je dobré dodržovat určitý styl.
Zde je ukázka uspořádání fotografií, na jaké byl program přizpůsoben:

* První foto: objekty rozmístěné v mřížce, jsou viditelné inventárním čísla.

* Druhé foto: stejné objekty se stejným rozmístěním, je vidět vrchní strana.
  Obsahuje také informaci o měřítku: 5cm značka nebo pravítko.

* Třetí (až n-té) foto: jednotlivé objekty popořadě zachycené z profilu.

# Grafické rozhraní

Pro zobrazení hlavních ovládacích prvků je nutné mít otevřenou databázi.

## Záložka Pisoar

Záložka slouží k vytvoření objektů v databázi a přiřazení jednotlivých pohledů z fotografií k objektům.

Levý seznam slouží k prohlížení složek a souborů typu .jpg, .png a .tif.
Je možné se pohybovat jen ve složce s databází a v jejích podsložkách.
Složky ve vyšších úrovních není možné prohlížet.
Díky tomu je ale celá databáze jednoduše přenositelná na jiné počítače.

Pod seznamem souborů jsou tlačítka pro správu tagů souborů.
Zvolený soubor lze otagovat pro lepší orientaci.

V pravé horní části okna je prostor pro náhled pohledu objektu.
Ten se zaplní při vybrání pohledu v hlavní části.

Pod náhledem pohledu je seznam objektů a ovládací prvky pro vytvoření objektu a základní práci se zvoleným objektem.
V seznamu objektů je možné objekt přejmenovat (stisknutím klávesy F2, nebo pomalým dvojklikem).

Na hlavní ploše se zobrazuje vybraný obrázek.
Práce na ploše má čtyři režimy, které lze změnit na horním panelu:

1. Posun plochy (metoda táhni a pusť)
2. Automatický výběr pohledu podle masky (metoda klikutí na objekt na fotografii ohraničený bílým okrajem)
3. Výběr pohledu pomocí obdélníku (metoda táhni a pusť)
4. Nastavení měřítka souboru (metoda táhni a pusť)

## Záložka Jaguar

Záložka slouží pro kontrolu a zobrazení objektu v jednotném stylu.

Vlevo je seznam objektů s možností filtrace nebo seřazení seznamu podle názvu objektu.

Na hlavní ploše se po vybrání objektu objeví jeho zobrazení v jednotném stylu.

## Záložka Kasuar

Záložka slouží k uspořádání více objektů na plochu a ke správě těchto ploch.

Vlevo je seznam objektů s tlačítkem pro přidání objektu na plochu, objekt lze na plochu přidat i pomocí dvojkliku nebo klávesy Enter.
Pokud je objekt umístěný na některé ploše, pro přehlednost změní svoji ikonu.

Vpravo je seznam ploch s tlačítkem pro přidání nové plochy a uložení plochy jako obrazový soubor.

Na spodním okraji plochy se automaticky generuje popisný text s čísly a názvy objektů.

Na ploše je možné:

 * Vybírat objekty jednotlivě (kliknutí myši)
 * Vybírat více objektů najednou pomocí výběrového čtverce (metoda táhni a pusť)
 * Přesouvat vybrané objekty na jiné místo (metoda táhni a pusť, směrové klávesy - šipky)
 * Odstranit objekty na ploše (klávesa Delete)
 * Přidávat do výběru / odebírat z výběru objekty jednotlivě (Ctrl+klinutí)
 * Zarovnat objekty k pomyslné mřížce při přesouvání (Ctrl+táhnutí)
 * Zobrazit u vybraných objektů měřítko nebo ohraničení (kliknutí na příslušné tlačítko na horním panelu)
 * Změnit měřítko u vybraných objetků (Ctrl+otáčením kolečka myši)

## Další informace

Všechny hlavní plochy na záložkách je možné přibližovat/oddalovat pomocí kolečka myši.

Výsledné plochy je možné uložit do souboru po stránkách, buď jednotlivě ručně tlačítkem "Uložit výsledek", nebo najednou automaticky v menu "Nástroje - Vygenerovat výstupy".

V menu "Nástroje - Nastavení" lze nastavit různé chování programu:

 * Vzdálenost pro získání měřítka z fotografie
 * Automatické posunutí výběru objektu na další při přiřazení pohledů
 * Možnost zobrazit nebo skrýt fotografie označené tagem "Hotové"

# Práce s programem

## Vytvoření nové databáze

Nejprve je potřeba vytvořit novou nebo otevřít již existující databázi.

Pro každou novou databázi program potřebuje vlastní složku.
Tuto složku je vhodné předem vytvořit a nakopírovat do ní všechny potřebné fotografie.
Po vytvoření databáze program vytvoří dvě podsložky: items a layouts.
Do složky items bude ukládat jednotlivé pohledy objektů a do složky layouts pak kompletní výstupy.

Vytvořenou a připravenou složku použijeme v programu pomocí menu "Databáze - Nová".
Pokud by databáze v této složce již existovala, program bude chtít potvrzení jejího přepsání.
Příště databázi budeme otevírat pomocí menu "Databáze - Otevřít" nebo pomocí seznamu posledních použitých databází.

## Vytvoření objektů v databázi programu

Na záložce "Pisoar" si v levém seznamu vybereme a zobrazíme fotografii s inventárními čísly.
Každé jméno objektu zapíšeme do kolonky vpravo, pro vytvoření klikneme na tlačítko "Nový objekt", nebo stiskneme Enter, což je rychlejší.
V databázi nelze mít dva objekty se stejným názvem.
Pokud se pokusíme vytvořit takový objekt, program nám to nepovolí.
To platí i pro přejmenování objektů, které se provádí takto: označíme objekt a stiskneme klávesu F2, nebo na něj pomalu klikneme ještě jednou.
Přepíšeme název a potvrdíme klávesou Enter.

Po vytvoření všech objektů z této fotografie můžeme fotografii označit tagem, že je již zpracovaná, kliknutím na tlačítko "Hotovo".

## Přiřazení pohledů k objektům

Nyní vybereme fotografii s vrchními pohledy a v pravém seznamu vybereme položku, ke kterému přísluší první objekt na fotografii.

- Pokud je objekt na fotografii ohraničený bílou barvou, vybereme nástroj "Maska" a klikneme myší někam doprostřed objektu na fotografii.
Program od tohoto bodu vytvoří masku až k bílému ohraničení.
V pravém horním rohu zkontrolujeme, zda jsme při ořezávání objektu neudělali nějakou chyu.

- Pokud objekt ohraničený není, vybereme nástroj "Čtverec" a označíme objekt od jednoho rohu k protilehlému rohu.

Pokud je vše v pořádku, můžeme masku přiřadit objektu kliknutím na tlačítko "Přiřadit masku".
V tom momentě se v adresáři items z masky vytvoří nový obrázek s názvem objektu a pořadovým číslem pohledu.
Program automaticky přeskočí na další objekt (v závistlosti na nastavení) a můžeme označit a přiřadit další masku.
Místo kliknutí myší na tlačítko "Přiřadit masku" je lepší použít klávesovou zkratku "Alt+P" (ideálně pravý "Alt+P", která se dá stisknout jednou rukou).

Pokud přiřadíme masku chybně, můžeme použít tlačítko "Odstranit pohledy", případě pohledy pouze přegenerovat.
To se hodí, když se vygeneruje maska s nějakým artefaktem a my si toho všimneme až po přiřazení objektu.
Pak příslušný soubor upravíme v editoru a tady klikneme na tlačítko "Prřegenerovat pohledy".
Přegenerují se nebo se odstraní všechny pohledy vybraného objektu.

Pod seznamem objektů nebo souborů je podrobnější info s křížovým odkazem na fotografie k pohledům nebo objekty v soboru.
Kliknutím na odkaz se označí příslušná položka na protější straně.

Důležité je také přiřadit k prvnímu pohledu měřítko.
Najdeme si tedy soubor s prvním pohledem objektu, vybereme nástroj "Měřítko" a metodou táhni-a-pusť označíme na fotografii 5cm úsek (popřípadě úsek jiné délky, pokud jsme velikost změnili v nastavení).
Vlevo se zobrazí konkrétní hodnota měřítka udaná v pixelech na centimetr.

Můžeme pokračovat přiřazením dalších pohledů k objektům.
Fotografii můžeme označit tlačítkem "Hotovo", opět je rychlejší použít klávesovou zkratku "Alt+H".

Tím máme připraveno několik objektů.

## Kontrola výstupu

Přepneme se na další záložku, která má název "Jaguar".
Tady si můžeme prohlédnout jednotlivě objekty tak, jak budou vypadat na výstupu.
Vlevo dole je okénko pro filtraci, takže lze rychle vyhledávat v databázi.
Tato záložka má zatím minimum funkcionality a čeká na nápad, jak ji rozšířit.
Možná o různé pohledy pro jeden objekt nebo jejich konkrétní podobu.

## Vyskládání objektů na stránku

Na poslední záložce s názvem "Kasuar" budeme vytvářet kompletované výstupy.
Vytvoříme tedy prázdnou stránku, můžeme ji i přejmenovat.

Přepneme se na seznam objektů a začneme vkládat objekty.
Buď pomocí klávesy Enter, nebo dvojkliku myší, nebo pomocí tlačítka "Přidat na plochu".
Každý objekt, který je na nějaké stránce, pro přehlednost změní svojí ikonu.
Objekty se umísťují střídavě na levou a na pravou polovinu plochy.

Objekty na ploše můžeme vybírat.
Vybraný objekt, nebo skupinu objektů můžeme různě upravovat.
Lze je posouvat po ploše pomocí šipek nebo táhnutím myší, přidržením Ctrl můžeme označovat nebo odznačovat další objekty nebo také vynutit přichycení k pomyslné mřížce při tažení myší.
Přidržením Ctrl a Scrollováním můžeme změnit velikost objektu, když je moc malý nebo naopak moc velký.
V těch případech je také vhodné k objektu přidat měřítko nebo rámeček, aby tato skutečnost byla na první pohled zřejmá.

Na spodním okraji stránky se automaticky generuje popisný text, který bude též součástí výstupu.
Výstupní stránka má rozměr o něco menší než formát A4.
Výsledek si můžeme uložit do souboru po stránkách, buď jednotlivě ručně tlačítkem "Uložit výsledek", nebo najednou automaticky v menu "Layout - Vygenerovat výstupy".

Tím jsme si ukázali hlavní rysy programu.

# Závěrečné poznámky

## Pohledy

Momentálně jsou k dispozici pouze tři pohledy - shora, shora + profil, shora + profil + strana.
Konkrétní pohled se vytvoří v závislosti na počtu pohledů objektu.

## Databáze

Databáze je jednoduchý strukturovaný textový soubor s názvem pisoardb.json.
Je možné jej ručně upravit v případech, kdy v programu chybí příslušná funkcionalita.
Při těchto úpravách však databáze nesmí být v programu otevřená a je doporučeno vytvořit zálohu.

Databázi je doporučeno při práci často ukládat a tento soubor také zálohovat v různých vezích.
Program je k dispozici bez jakékoliv záruky, může obsahovat chyby, které způsobí náhlé ukončení programu, v krajním případě poškození databáze.
Takovéto chování hlašte s podrobnostmi na emailovou adresu uvedenou níže.

## Kompatibilita

V zabaleném archivu by měly být všechny potřebné soubory ke spuštění na systému Windows 7, na jiných systémech kromě Linuxu program nebyl testován.
Program by však měl fungovat i na novějších verzích systému Windows díky tomu, že využívá univerzální knihovnu Qt.

## Nové verze

Nejnovější verzi programu Pisoar je možné stáhnout na adrese [http://www.spinler.cz/pisoar](https://www.spinler.cz/pisoar)


Zdrojové soubory programu jsou volně ke stažení na adrese [https://github.com/martinspinler/pisoar](https://github.com/martinspinler/pisoar)

Vytvořil Martin Špinler.
Dotazy, náměty k vylepšení a reporty chyb pište na [martin.spinler@gmail.com](mailto:martin.spinler@gmail.com)
