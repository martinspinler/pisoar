# Úvod

Tento program usnadňuje vytváření strukturovaných obrazových výstupů z fotografií objektů.

Program se jmenuje Pisoar, což je zkratka odvozená od spojení "PIcture SOrter for ARchaeology".

# Příprava vstupu

Než se pustíme do samotné práce s Pisoárem, je potřeba pro něj připravit vstup. Vstupem budou Vaše fotografie nebo obrázky, na nichž jsou zachycené objekty z vrchu nebo z profilu. Objekty na těchto fotografiích je nutné oříznout tak, aby měly kolem sebe několik pixelů čistě bílé barvy. Podle toho program rozpozná hranice objektu na fotografii. Je nutné, aby to byla úplně bílá barva, jinou okrajovou barvu program zatím nedokáže zpracovat. Je také dobré dát si pozor na to, aby k objektu nezůstala vést nějaká nitka jiné barvy, ale to vše jsou chyby, které v Pisoáru hned uvidíte a zpětně v editoru fotografií opravíte.

Program je trochu přizpůsobený vstupu, který jsem dostal já, takže pokud fotografie teprve budete pořizovat, můžete se inspirovat stejným stylem. Práce Vám pak půjde rychleji od ruky:

- První foto obsahovalo několik objektů s inventárním číslem. Podle inventárního čísla vytvoříme objekty v interní databázi programu.

- Druhé foto obsahovalo ty samé objekty, stejně rozložené, jen otočené tak, aby byla vidět vrchní strana, kterou chceme mít na výstupu. Na fotografii bylo také pravítko, které je důležité pro vypočítání měřítka objektu. Postačí 5 cm značka.

- Třetí až n-té foto obsahovalo popořadě ty samé objekty, ale zachycené z profilu.

# Spuštění programu a vytvoření databáze

Nyní už můžeme spustit samotný program. V zabaleném archivu by měly být všechny potřebné soubory ke spuštění na systému Windows 7, na jiných systémech kromě Linuxu program nebyl testován, přesto je pravděpodobné, že bude fungovat také, díky univerzální knihovně Qt.

Program ke své činnosti potřebuje nějakou složku, kde si vytvoří databázi. Databáze je vlastně strukturovaný textový soubor s názvem pisoardb.json. Složku si tedy vytvoříme a použijeme v programu takto: menu Databáze-Nová. Pokud by databáze v této složce již existovala, program bude chtít potvrzení přepsání.

Po vytvoření databáze program vytvoří dvě podsložky: items a layouts. Do první složky bude ukládat jednotlivé pohledy objektů a do druhé pak kompletní výstupy.

Tyto složky se také již zobrazí v levém seznamu, který funguje jako prohlížeč souborů a složek. Momentálně jsou vidět jen složky a obrázky typu .jpg a .png. Je možné prohlížet soubory jen ve složce s databází a její podsložky. Složky ve vyšších úrovních není možné prohlížet, ale díky tomu je celá databáze i s fotografiemi přenosná na jiné počítače.

Pokud jsme databázi nevytvářeli v existující složce s připravenými fotografiemi, musíme si nyní do této složky připravené fotografie nakopírovat. Obnovíme seznam souborů, třeba otevřením jiné složky a vrátíme se zpět a můžeme začít tvořit objekty.

# Vytvoření objektů v databázi programu

Jako první si zobrazíme fotografii s inventárními čísly. Každé jméno objektu zapíšeme do kolonky vpravo, pro vytvoření klikneme na tlačítko "Nový objekt", nebo stiskneme Enter, což je rychlejší. V databázi nelze mít dva objekty se stejným názvem. Pokud se pokusíme vytvořit takový objekt, program nám to nepovolí. To platí i pro přejmenování objektů, které se provádí takto: Označíme objekt a stiskneme F2, nebo na něj klikneme ještě jednou \(ale ne rychlým dvojklikem\). Přepíšeme název a potvrdíme Entrem.

Až budeme mít vytvořené objekty z této fotografie, můžeme si ji označit tagem, že je již zpracovaná.

Samozřejmě můžeme jména objektů přepsat z jiného zdroje, ale importovat databázi objektů z jiné databáze zatím program neumí.

# Přiřazení pohledů k objektům

Nyní se přepneme na fotografii s vrchními pohledy a v pravém seznamu vybereme položku, ke kterému přísluší první objekt na fotografii.

Klikneme myší někam doprostřed objektu na fotografii. Program od tohoto bodu vytvoří masku až k bílému ohraničení. V pravém horním rohu zkontrolujeme, zda jsme při ořezávání objektu neudělali nějakou chyu. Pokud je vše v pořádku, můžeme masku přiřadit objektu kliknutím na tlačítko "Přiřadit masku". V tom momentě se v adresáři items z masky vytvoří nový obrázek s názvem objektu a pořadovým číslem pohledu. Program automaticky přeskočí na další objekt a můžeme označit a přiřadit další masku. Klik, Přiřadit masku. Já na tlačítko Přiřadit masku neklikám myší, ale používám klávesovou zkratku Alt+P, nebo lépe pravý Alt+P, která se dá stisknout jednou rukou. Práce je tak rychlejší.

Pokud přiřadíme masku chybně, můžeme použít tlačítko "Odstranit pohledy", případě pohledy pouze přegenerovat. To se hodí, když se vygeneruje maska s nějakým artefaktem a my si toho všimneme až po přiřazení objektu. Pak příslušný soubor upravíme v editoru a tady klikneme na přegenerovat. Přegenerují se nebo se odstraní všechny pohledy vybraného objektu.

Pod seznamem objektů nebo souborů je podrobnější info s křížovým odkazem na fotografie k pohledům nebo objekty v soboru. Kliknutím se označí příslušná položka na protější straně.

Důležité je také přiřadit k prvnímu pohledu měřítko. Najdeme si tedy soubor s prvním pohledem objektu, klikneme na tlačítko "Nastavit měřítko" a metodou táhni-a-pusť označíme na fotografii 5cm úsek. Vlevo se zobrazí konkrétní hodnota měřítka udaná v pixelech na centimetr.

Můžeme pokračovat přiřazením dalších pohledů k objektům. Vybereme první objekt a fotografii. Klikneme doprostřed objektu na fotografii a přiřadíme masku. Fotografii můžeme označit jako hotovou \(to lze i pomocí zkratky Alt+H\).

Tím máme připraveno několik objektů.

# Kontrola výstupu

Přepneme se na další záložku, která má název Jaguar. Tady si můžeme prohlédnout jednotlivě objekty tak, jak budou vypadat na výstupu. Vlevo dole je okénko pro filtraci, takže lze rychle vyhledávat v databázi. Tato záložka má zatím minimum funkcionality a čeká na nápad, jak ji rozšířit. Možná o různé pohledy pro jeden objekt nebo jejich konkrétní podobu.

# Vyskládání objektů na stránku

Na poslední záložce s názvem Kasuar budeme vytvářet kompletované výstupy. Vlevo je databáze objektů, vpravo je seznam stránek. Vytvoříme si tedy prázdnou stránku, můžeme ji i přejmenovat. Přepneme se na seznam objektů a začneme vkládat objekty. Buď pomocí Enter, nebo dvojkliku, nebo pomocí tlačítka "Přidat na plochu". Jak je vidět, každý objekt, který je na nějaké stránce, pro přehlednost změní svojí ikonu. Objekty se umísťují střídavě na levou a na pravou polovinu. Vždy se umístí pod již existují "řádek". I když tento způsob má své mouchy.

Objekty na ploše můžeme vybírat. Vybraný objekt, nebo skupinu objektů můžeme různě upravovat. Lze je posouvat po ploše pomocí šipek nebo táhnutím myší, přidržením Ctrl můžeme označovat nebo odznačovat další objekty nebo také vynutit přichycení k pomyslné mřížce při tažení myší. Přidržením Ctrl a Scrollováním můžeme změnit velikost objektu, když je moc malý nebo naopak moc velký. V těch případech je také vhodné k objektu přidat měřítko nebo rámeček, aby tato skutečnost byla na první pohled zřejmá.

Na spodním okraji stránky se automaticky generuje popisný text, který bude též součástí výstupu. Výstupní stránka má rozměr o něco menší než formát A4. Výsledek si můžeme uložit do souboru po stránkách, buď jednotlivě ručně tlačítkem "Uložit výsledek", nebo najednou automaticky v menu Layout-Vygenerovat výstupy.

Tím jsme si ukázali hlavní rysy programu.

# Poznámky na závěr

Momentálně jsou k dispozici pouze tři pohledy - shora, shora + profil, shora + profil + strana. Konkrétní pohled se vytvoří v závislosti na počtu pohledů objektu.

Program dávám k dispozici bez jakékoliv záruky, databázi \(což je soubor pisoardb.json\) doporučuji často ukládat a tento soubor zálohovat v různých vezích, protože program není bezchybný a může padat, v krajním případě si může svoji databázi i poškodit.

Rád bych Vás poprosil o jakoukoliv zpětnou reakci – poznámky k používání, vylepšení, podoba výstupů, nějaký chybějící nástroj, nebo chyba v programu. Můžete také program rozšířit mezi další osoby, které jej využijí. Pokud Vám program pomůže, můžete mě pozvat na pivo, nebo mi alespoň napsat, že jej používáte, ať mám chuť do dalších úprav nebo podobných projektů.

 
Nové verze programu Pisoár stahujte zde:

[http://www.spinler.cz/pisoar](http://www.spinler.cz/pisoar)

Zdrojové soubory programu jsou ke stažení zde:

[https://github.com/martinspinler/pisoar](https://github.com/martinspinler/pisoar)

Díky,

Martin Špinler, [martin.spinler@gmail.com](mailto:martin.spinler@gmail.com)

