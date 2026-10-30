#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* rbyte ειναι βοηθητική συνάρτηση για την υλοποιήση όλου του προγραμματος */
int rbyte(void) { /* η συνάρτηση διαβάζει ένα byte απο την stdin και επιστρέφει τιμές απο 0-255*/
    int c = getchar();

    if (c == EOF) { /*αν φτάσει σε EOF πριν διαβαστούν όλα τα απαιτούμενα bytes ,το προγραμμα τερματίζεται με σφάλμα */ 
        fprintf(stderr, "Error! unexpected end of file\n");
        exit(1); 
    }

    return c & 0xFF;   /* κρατάμε μόνο τα 8 bits, αγνοούνται οι αρνητικές τιμές */
}

int wbyte(int c) {/*δίνουμε μεταβλητή ώστε να το στείλει στο output*/
    if (putchar(c) == EOF) { /*η putchar() γράφει 1 byte στο stdout*/ 
        fprintf(stderr, "Error! write failed\n");
        exit(1);/*αν υπάρχει πρόβλημα και δεν μπορεί να το εμφανίσει το πρόγραμμα τερματίζεται με σφάλμα*/
    }
    return c & 0xFF; /*αυτό εξασφαλίζει ότι παίρνουμε τα χαμηλότερα 8 bits και αγνοούνται οι αρνητικές τιμές */
}

unsigned short b2(void) {/*χρησιμοποιώ short αφου το αποτέλεσμα(out) θα είναι μέχρι τα 16 bits*/
    int b0 = rbyte();   /*Διαβάζουμε 2 bytes και τα αποθηκευουμε σε little-endian μορφή*/
    int b1 = rbyte();  

    unsigned short out = (unsigned short)(b0/*δεν μετακινείται (0-7)*/ | (b1 << 8));//κάνει shift 8 θέσεις, δηλ. μετακινεί τα bits(8-15)*/ 
                                                                    /* (bitwise OR) ενώνει τα bytes σε ένα ενιαιο αριθμό */
    return out; 
}
/*με την ίδια λογική δουλεύει και η συνάρτηση b4 με τη διαφορά ότι επιστρέφει int σε 32-bit τιμή και όχι short  */
unsigned int b4(void) {
    int b0 = rbyte();  
    int b1 = rbyte();
    int b2 = rbyte();
    int b3 = rbyte();  

    unsigned int out =  /* εδώ ενώνονται τα bytes σε ένα ενιαίο ακέραιο 8*/
        (unsigned int)b0 |     /* δεν μετακινείται κάτι ,μένει όπως είναι (0-7)*/
        ((unsigned int)b1 << 8) |   /*κάνει shift 8 και πάει στη 2η θέση(8-15) */
        ((unsigned int)b2 << 16) |  /* κάνει shift 16 και πάει στη 3η θέση (16-23)*/
        ((unsigned int)b3 << 24);   /* κάνει shift 24 και πάει στη 4η θέση (24-31)*/

    return out;
}

/* Γράφει έναν unsigned int (4 bytes) σε little-endian μορφή */
void w4(unsigned int v) {   /* δεν επιστρέφει κάτι */
    /* γράφουμε πρώτα το λιγότερο σημαντικό byte (bits 0–7) και καλουμε την wbyte ώστε να τυπσώσει στο wav το αποτελεσμα  */
    wbyte(v & 0xFF);

    /* μετά το επόμενο byte (bits 8–15) */
    wbyte((v >> 8) & 0xFF);

    /* μετά το τρίτο byte (bits 16–23) */
    wbyte((v >> 16) & 0xFF);

    /* και τελευταίο το πιο σημαντικό byte (bits 24–31) */
    wbyte((v >> 24) & 0xFF);
}

/* Γράφει έναν unsigned short (2 bytes) σε little-endian μορφή */
void w2(unsigned short v) {
    /* πρώτα το λιγότερο σημαντικό byte (bits 0–7) */
    wbyte(v & 0xFF);

    /* μετά το δεύτερο byte (bits 8–15) */
    wbyte((v >> 8) & 0xFF);
}

void info(void) {
    /*  Έλεγχος για "RIFF" */
    int tempriff[4],i;
    for(i=0;i<4;i++){/* Διαβάζουμε τα πρώτα 4 bytes του αρχείου*/
        tempriff[i]=rbyte();
    }
    /*εδώ γίνεται έλεγχος αν το αρχείο περιέχει bytes που σε ASCII έχουν την μορφή RIFF*/
    if (tempriff[0] != 'R' || tempriff[1] != 'I' || tempriff[2] != 'F' || tempriff[3] != 'F') {
        fprintf(stderr, "Error! \"RIFF\" not found\n");
        exit(1);/*αν υπάρχει σφάλμα το προγραμμα τερματίζεται */
    }
    
    /*Size of file*/
    unsigned int filesize=b4();
    printf("size of file: %u\n", filesize);/*χρησιμοποιω %u επειδή η τιμή είναι unsigned δηλ. τυπώνονται μονο θετικοί*/
    
    /* Έλεγχος για "WAVE" */
    int tempwave[4];
    for (i = 0; i < 4; i++) {   /* διαβάζουμε τα 4 bytes της λέξης WAVE */
        tempwave[i] = rbyte();
    }
    /*με την ίδια λογικη του RIFF γίνεται έλεγχος αν καποιο byte δεν αντιστοιχεί στους πιο κάτω ASCII χαρακτήρες*/ 
    if (tempwave[0] != 'W' || tempwave[1] != 'A' ||tempwave[2] != 'V' || tempwave[3] != 'E') {
        fprintf(stderr, "Error! \"WAVE\" not found\n");
        exit(1);/* αν βρεθεί σφάλμα στη συνθήκη το προγραμμα τερματίζεται */
        }
    
        /* Έλεγχος για "fmt " */
    int tempfmt[4];
    for (i = 0; i < 4; i++) {      /* διαβάζουμε τα 4 bytes του "fmt "*/
        tempfmt[i] = rbyte();
    }

    if (tempfmt[0] != 'f' || tempfmt[1] != 'm' ||tempfmt[2] != 't' || tempfmt[3] != ' ') {/*ελέγχος και για τον τελευταιο χαρακτήρα αν είναι κενο όπως το περιγράφει η άσκηση */
        fprintf(stderr, "Error! \"fmt \" not found\n");
        exit(1);
    }
    
        /* Μέγεθος format chunk */
    unsigned int fmtsize = b4();
    
    printf("size of format chunk: %u\n", fmtsize);
    
    if (fmtsize != 16) { /* με βάση τις οδηγίες το μέγεθος πρέπει να είναι 16*/
        fprintf(stderr, "Error! size of format chunk should be 16\n");
        exit(1);
    }

    /* type of WAVE format (2 bytes, πρέπει να είναι 1 για PCM) */
    unsigned short wavetype = b2(); /*χρησιμοποιούμαι την συγκεκριμένη συνάρτηση γιατί απαιτούνται μόνο 2 bytes*/
    
    printf("WAVE type format: %u\n", wavetype);
    
    if (wavetype != 1) {/* οι οδηγίες υπογραμμίζουν ότι ο τύπος πρέπει να ισουται με 1 οταν αποθηκεύεται σε μορφή little endian */
        fprintf(stderr, "Error! WAVE type format should be 1\n");
        exit(1);
    }
    
    /*Ελεγχος MonoStereo*/
    
    /* Διαβάζουμε το πεδίο "number of channels" (2 bytes, σε μορφή little-endian).
    Η τιμή αυτή δείχνει αν ο ήχος είναι mono (1) ή stereo (2). */
    unsigned short channels = b2();
    
    printf("mono/stereo: %u\n", channels);

    if (channels != 1 && channels != 2) {/* η άσκηση επιτρέπει μόνο 1 (mono) ή 2 (stereo).
                                         Οποιαδήποτε άλλη τιμή θεωρείται μη υποστηριζόμενη μορφή WAV*/
    fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
    exit(1);/*το πρόγραμμα τερματίζεται λόγω λάθους στο header με σφάλμα */
    }
    
    /*Sample Rate*/
    
    unsigned int samplerate = b4(); /*Αυτό θα σου διαβάσει τα 4 bytes σε little-endian*/
    printf("sample rate: %u\n", samplerate);/*εμφάνιση τιμής που δείχνει πόσα δείγματα ήχου παίρνονται ανά δευτερόλεπτο*/
    
    /*BytesPerSecond*/
    
    /*4 bytes, σε μορφή little-endian και δείχνει πόσα bytes απαιτούνται για 1 δευτερόλεπτο ήχου*/
    unsigned int byterate = b4();

    printf("bytes/sec: %u\n", byterate);
    
    
    /*BlockAlign*/
    
    /* καλούμε την συγκεκριμένη συνάρτηση γιατί έχουμε 2 bytes και θέλουμε να τα αποθηκεύσουμε σε little-endian)*/
    unsigned short blockalign = b2();

    printf("block alignment: %u\n", blockalign);
    
    /* Έλεγχος ότι bytes/sec = sample_rate * block_align */
    if (byterate != samplerate * blockalign) {
    fprintf(stderr, "Error! bytes/second should be sample rate x block alignment\n");
    exit(1);
    }
    
    /*BitsPerSample*/
    
    unsigned short bitspersample = b2();

    /* Εμφάνιση των bits ανά sample. */
    printf("bits/sample: %u\n", bitspersample);
    
     if (bitspersample != 8 && bitspersample!= 16)  {
    fprintf(stderr, "Error! bits/sample should be 8 or 16\n");
    exit(1);
    }
    
    /* Έλεγχος ότι blockalign = (bitspersample/8) * channels */
    if (blockalign != (bitspersample / 8) * channels) {
    fprintf(stderr, "Error! block alignment should be bits per sample / 8 x mono/stereo\n");
    exit(1);
    }
     /*DataChunk*/
    
    char data[4];
    for (i = 0; i < 4; i++) {
        data[i] = rbyte();/*αποθηκευση κάθε χαρακτήρα ASCII ώστε να τους ελέγξουμε στην συνέχεια */ 
    }
    
      if (data[0] != 'd' || data[1] != 'a' ||data[2] != 't' || data[3] != 'a') {
        fprintf(stderr, "Error! \"data\" not found\n");
        exit(1);/*αν κάποιος χαρακτήρας δεν αντιστοιχεί στο ζητούμενο το πρόγραμμα τερματίζεται με σφάλμα */
    }
    
    /*SizeOfData*/
    
    unsigned int datasize = b4();/* καλούμε την συγκεκριμένη συνάρτηση γιατί ακολουθούν 4  bytes και θέλουμε να τα αποθηκεύσουμε σε little-endian)*/
    printf("size of data chunk: %u\n", datasize);
    
    /*ελέγχοι με βάση το data size που διαβάσαμε πριν με την b4*/
    /* Προσπαθούμε να διαβάσουμε ακριβώς τον αριθμό datasize bytes δεδομένων ήχου */
    int unsigned j;
    
    for (j = 0; j < datasize; j++) { 
    int c = getchar();   // διάβασμα ενός byte κάθε φορά που εκτελείται η for απο το αρχείο*/ 
    if (c == EOF) { /*αν φτάσει σε EOF πριν να διαβάσει τον αριθμό των bytes που πρέπει με βάση το size που διαβάσαμε πριν*/
        fprintf(stderr, "Error! insufficient data\n");/*το πρόγραμμα τερματίζεται με σφάλμα αφού το αρχείο είναι ελλειπής*/ 
        exit(1);
        }
    }
    
    /* Ελέγχουμε αν υπάρχει παραπάνω data από όσο λέει το datasize */
    unsigned int extradata=filesize-36-datasize;

    for(j=0;j<extradata;j++){
        int c = getchar();   // διάβασμα ενός byte κάθε φορά που εκτελείται η for απο το αρχείο*/ 
        if (c == EOF) { /*αν φτάσει σε EOF πριν να διαβάσει τον αριθμό των bytes που πρέπει με βάση το size που διαβάσαμε πριν*/
        fprintf(stderr, "Error! insufficient data\n");/*το πρόγραμμα τερματίζεται με σφάλμα αφού το αρχείο είναι ελλειπής*/ 
        exit(1);
        }
    }
    int extra = getchar();/* κανονικά αυτη η μεταβλητή πρέπει αντιστοιχεί με EOF*/
    if (extra != EOF  ) { /* αν δεν αντιστοιχεί με EOF τότε σημαίνει πως υπάρχουν περισσότερα δεδομένα στο αρχείο αποτι θα έπρεπε και σε αυτή την περίπτωση το προγραμμα τερματίζεται με σφάλμα*/
    fprintf(stderr, "Error! bad file size (found data past the expected end of file)\n");
    exit(1); 
    }
     
}

void rate(double factor) {
    if (factor <= 0.0) { /*δεν μπορούμε να δώσουμε αρνητικό αριθμό (δεν θέλουμε reverse του ήχου)*/
        fprintf(stderr, "Error! rate factor must be positive\n");
        exit(1);/*αν δωθεί αρνητικός το πρόγραμμα τερματίζεται με σφάλμα*/
    }

    int i;
    
        /*  Έλεγχος και αντιγραφή του "RIFF"*/
    int riff[4];

    for (i = 0; i < 4; i++) {
        riff[i] = rbyte();
    }
    if (riff[0] != 'R' || riff[1] != 'I' ||riff[2] != 'F' || riff[3] != 'F') {
        fprintf(stderr, "Error! \"RIFF\" not found\n");
        exit(1);
    }
      /* αν όλα είναι σωστά, γράφουμε ξανά το "RIFF" στο output του νέου αρχείου wav το οποίο δημιουργείται ταυτόχρονα*/
    wbyte('R');
    wbyte('I');
    wbyte('F');
    wbyte('F');
    
    /*FileSize*/
    unsigned int filesize = b4();/*όπως στην info διαβάζουμε τα 4 επομενα bytes με την b4*/
     w4(filesize);/*με την w4 γράφουμε στο output (stdout) το αποτέλεσμα που επιστρέφεται */
    
        /*Έλεγχος και αντιγραφή του "WAVE" */
    int wave[4];

     for (i = 0; i < 4; i++) {/* διαβάζουμε τα επόμενα 4 bytes από το wav */
        wave[i] = rbyte();
    }
    if (wave[0] != 'W' || wave[1] != 'A' ||wave[2] != 'V' || wave[3] != 'E') {/* ελέγχουμε ότι είναι οι χαρακτήρες 'W', 'A', 'V', 'E'(ASCII) */
        fprintf(stderr, "Error! \"WAVE\" not found\n");
        exit(1);
    }
     /* αν όλα είναι σωστά, γράφουμε ξανά το "WAVE" στο output */
    wbyte('W');
    wbyte('A');
    wbyte('V');
    wbyte('E');
    
        /*  Έλεγχος και αντιγραφή του "fmt " */
    int fmt[4];
    for (i = 0; i < 4; i++) {
        fmt[i] = rbyte();
    }
    if (fmt[0] != 'f' || fmt[1] != 'm' ||fmt[2] != 't' || fmt[3] != ' ') {/* ελέγχουμε ότι είναι οι χαρακτήρες 'f', 'm', 't', ' '  */
        fprintf(stderr, "Error! \"fmt \" not found\n");
        exit(1);
    }
    /* γράφουμε ξανά το "fmt " στο output του νέου WAV */
    wbyte('f');
    wbyte('m');
    wbyte('t');
    wbyte(' ');
    
        /*fmt size (4 bytes) */
    unsigned int fmtsize = b4();   /* διαβάζουμε το μέγεθος του fmt chunk το οποίο με βάση την ασκηση πρέπει να ειναι 16*/

    /* πρέπει να είναι 16 για PCM */
    if (fmtsize != 16) {
        fprintf(stderr, "Error! size of format chunk should be 16\n");
        exit(1);
    }

    w4(fmtsize);  /* το ξαναγράφουμε στο output όπως είναι (16) καλώντας την w4 */
    
        /* wave type */
    unsigned short wavetype = b2();   /* διαβάζουμε την μορφή του ήχου με την w2 αφου θέλουμε να διαβάσουμε τα επόμενα 2 bytes του wav */

    if (wavetype != 1) {/* πρέπει να είναι 1 για PCM με βάση την ασκηση*/
        fprintf(stderr, "Error! WAVE type format should be 1\n");
        exit(1);
    }
     w2(wavetype);/* γράφουμε την ίδια τιμή στο output του νέου αρχείου αφού βεβαιωθήκαμε ότι είναι εγκυρη */
    
        /* channels (mono/stereo) */
    unsigned short channels = b2();   /* διαβάζουμε πόσα κανάλια έχει 1για mono, 2=stereo όπως και στην info */

    if (channels != 1 && channels != 2) {
        fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
        exit(1);
    }

    /* γράφουμε ξανά τα channels στο output του νέου wav */
    w2(channels);
    
    /*Sample Rate*/
    /* εδώ θα χρησιμοποιήσουμε τη μεταβλητή factor που έδωσε ο χρήστης ώστε να μεταβάλουμε τον ήχο */
    unsigned int oldrate = b4();/* Διαβάζουμε το παλιό sample rate (4 bytes little-endian) όπως το διαβάσαμε στην info */
    unsigned int newrate = (unsigned int)(oldrate * factor);/* Υπολογίζουμε το νέο sample rate και βάζουμε μπροστά unsigned int για να διατηρηθεί η little-endian μορφή  */

    w4(newrate);/* Το γράφουμε στο νέο αρχείο το νέο sample rate με το οποίο θα μεταβάλλεται πως παίζεται ο ήχος  */
    
    /* bytes per sec */
    
    unsigned int oldbyterate = b4();/* διαβάζουμε το παλιό byte_rate από το input σε 4 bytes little-endian μορφή όπως κάναμε στην info */
    unsigned int newbyterate = (unsigned int)(oldbyterate * factor);/* υπολογίζουμε το νέο byte rate πολλαπλασιάζοντας με τον factor που έδωσε ο χρήστης  */

    w4(newbyterate); /* γράφουμε το νέο byterate στο output του νέου wav */
    
    /*block alignment */
    unsigned short blockalign = b2();
    w2(blockalign);

    /* bits per sample  */
    unsigned short bitspersample = b2();
     if (bitspersample != 8 && bitspersample!= 16)  {
    fprintf(stderr, "Error! bits per sample should be 8 or 16\n");
    exit(1);
    }
    w2(bitspersample);
    
     /* data chunk */
    int data[4];
    for (i = 0; i < 4; i++) {
        data[i] = rbyte();
    }

    if (data[0] != 'd' || data[1] != 'a' || data[2] != 't' || data[3] != 'a') {
        fprintf(stderr, "Error! \"data\" not found\n");
        exit(1);
    }

    wbyte('d'); 
    wbyte('a'); 
    wbyte('t'); 
    wbyte('a');

    /*size of data */
    unsigned int datasize = b4();
    w4(datasize);

    int unsigned j;
    
    for (j = 0; j < datasize; j++) { 
    int c = getchar();   // διάβασμα ενός byte κάθε φορά που εκτελείται η for απο το αρχείο*/ 
    if (c == EOF) { /*αν φτάσει σε EOF πριν να διαβάσει τον αριθμό των bytes που πρέπει με βάση το size που διαβάσαμε πριν*/
        fprintf(stderr, "Error! insufficient data\n");/*το πρόγραμμα τερματίζεται με σφάλμα αφού το αρχείο είναι ελλειπής*/ 
        exit(1);
        }
        putchar(c);
    }
    
    /* Ελέγχουμε αν υπάρχει παραπάνω data από όσο λέει το datasize */
    unsigned int extradata=filesize-36-datasize;

    for(j=0;j<extradata;j++){
        int c = getchar();   // διάβασμα ενός byte κάθε φορά που εκτελείται η for απο το αρχείο*/ 
        if (c == EOF) { /*αν φτάσει σε EOF πριν να διαβάσει τον αριθμό των bytes που πρέπει με βάση το size που διαβάσαμε πριν*/
        fprintf(stderr, "Error! insufficient data\n");/*το πρόγραμμα τερματίζεται με σφάλμα αφού το αρχείο είναι ελλειπής*/ 
        exit(1);
        }
        putchar(c);
    }
    int extra = getchar();/* κανονικά αυτη η μεταβλητή πρέπει αντιστοιχεί με EOF*/
    if (extra != EOF  ) { /* αν δεν αντιστοιχεί με EOF τότε σημαίνει πως υπάρχουν περισσότερα δεδομένα στο αρχείο αποτι θα έπρεπε και σε αυτή την περίπτωση το προγραμμα τερματίζεται με σφάλμα*/
    fprintf(stderr, "Error! bad file size (found data past the expected end of file)\n");
    exit(1); 
    }

}
void channel( char *mode){
    int i;

    /* Έλεγχος αν ο χρήστης έγραψε left ή right καλώντας την συνάρτηση strcmp η οποία ελέγχει αν το string που έδωσε ο χρήστης είναι το ίδιο με left ή right */
    if (strcmp(mode, "left") != 0 && strcmp(mode, "right") != 0) {
        fprintf(stderr, "Error! channel mode must be \"left\" or \"right\"\n");
        exit(1);/*αν ο χρήστης δεν δώσει left ή right το πρόγραμμα τερματίζεται με σφάλμα*/
    }
     /*RIFF*/
    int riff[4];/*Διαβάζουμε τα 4 πρώτα bytes απο το αρχείο wav όπωσ κάναμε στην info και στην rate  */
    for (i = 0; i < 4; i++) {
        riff[i] = rbyte();  
    }
    if (riff[0] != 'R' || riff[1] != 'I' ||riff[2] != 'F' || riff[3] != 'F') {
        fprintf(stderr, "Error! \"RIFF\" not found\n");
        exit(1);
    }
    /*File Size */
    unsigned int filesize = b4();   /* διαβάζουμε το παλιό file size από το input */
      /* WAVE */
    int wave[4];
    for (i = 0; i < 4; i++) {
        wave[i] = rbyte(); 
    }

    if (wave[0] != 'W' || wave[1] != 'A' || wave[2] != 'V' || wave[3] != 'E') {
        fprintf(stderr, "Error! \"WAVE\" not found\n");
        exit(1);
    }
    /* "fmt " */
    int fmt[4];
        
     for (i = 0; i < 4; i++) {/* διαβάζουμε τα 4 bytes του "fmt " */
            fmt[i] = rbyte();
    }

    if (fmt[0] != 'f' || fmt[1] != 'm' || fmt[2] != 't' || fmt[3] != ' ') {/* έλεγχoyme ότι είναι οι χαρακτήρες 'f', 'm', 't', ' ' */
            fprintf(stderr, "Error! \"fmt \" not found\n");
            exit(1);
     }

     /*  fmt size */
    unsigned int fmtsize = b4();   /* διαβάζουμε τα 4 bytes */

    if (fmtsize != 16) { /*πρέπει να είναι 16 αλλιώς το αρχείο δεν είναι έγκυρο και το προγραμμα θα τερματίζεται με σφάλμα*/
        fprintf(stderr, "Error! fmt chunk size should be 16\n");
        exit(1);

    }

    /*wave type*/
    unsigned short wavetype = b2();   /* διαβάζουμε 2 bytes για τον τύπο ήχου */
      if (wavetype != 1) {/* πρέπει να είναι 1 για PCM */
        fprintf(stderr, "Error! WAVE type format should be 1 (PCM)\n");
        exit(1);
    }
    
    /* channels */
    /*εδώ με βάση αυτο που θα διαβασουμε θα δουμε αν πρέπει να κανουμε αλλαγές στο νέο αρχείο που θα δημιουργηθεί η όχι*/
    unsigned short channels = b2();   /* διαβάζουμε πόσα κανάλια έχει το input */

    if (channels != 1 && channels != 2) {/* Έλεγχος ότι είναι 1 ή 2 αλλιώς το πρόγραμμα θα τερματιστεί*/
        fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
        exit(1);
    }

    /* ορίζουμε μια νέα μεταβλητή με την οποία θα δουλέψουμε ώστε αν διαβάσαμε 2 απο το αρχείο να  */
    unsigned short channels_output;

    if (channels == 1) {
        channels_output = 1;/*αν είναι 1 τότε δεν μεταβάλλουμε κάτι και η νέα μεταβλητή μας ισούται με 1 και τα samples δεν θα αλλάξουν */
    }

    else if(channels==2){
        /*αν ειναι 2 τότε η νέα μεταβλητή θα ισούται με 1 (δηλ. μετατρέπουμε τα channels σε mono) ώστεε να πάρουμε μόνο το ένα κανάλι που ζητησε ο χρήστης*/
        channels_output = 1;
    }


    /* Αν είναι σωστό, γράφουμε "RIFF" στην έξοδο του νέου αρχείου που δημιουργείται αυτόματα*/
    wbyte('R');
    wbyte('I');
    wbyte('F');
    wbyte('F');

    unsigned int filesize_output;
    if(channels==1){
        filesize_output=filesize;
    }
    else if(channels==2){
        filesize_output=(filesize+36)/2;
    }
    w4(filesize_output);/* προς το παρόν το κρατάμε ίδιο και στο output */

    /* γράφουμε "WAVE" στο output του νέου αρχείου */
    wbyte('W');
    wbyte('A');
    wbyte('V');
    wbyte('E');
  
    wbyte('f');
    wbyte('m');
    wbyte('t');
    wbyte(' ');

    w4(fmtsize);

    w2(wavetype);

    
    /* γράφουμε το out_channels στο αρχείο εξόδου */
    w2(channels_output);

    /* Sample Rate, Byte Rate, Block Align, Bits Per Sample */
    unsigned int samplerate  = b4();  
    unsigned int byterate = b4();  
    unsigned short blockalign = b2(); 
    unsigned short bitspersample = b2();
    
    /* εδώ υπολογίζουμε τις τιμές των μεταβλητών που θα τυπωθούν στο νέο wav */
    unsigned short blockalign_output;
    unsigned int byterate_output;

    if (channels == 1) {/* Αν το input είναι mono και δεν αλλάζουμε τίποτα */
         blockalign_output = blockalign;
         byterate_output = byterate;

    } 
    else {
        /* Αν το input είναι stereo πρέπει να μετατραπεί σε mono (και θα υπάρξουν αλλαγές) */
        blockalign_output = (bitspersample / 8) * channels_output;
        byterate_output = samplerate * blockalign_output;
    }
    
    w4(samplerate);/* sample rate δεν αλλάζει στην channel */

    w4(byterate_output);/* γράφουμε το νέο byte rate (ίδιο ή υπολογισμένο, ανάλογα με mono/stereo) */

    w2(blockalign_output);/* γράφουμε το νέο block align στο νέο αρχείο wav */

    w2(bitspersample);/* bits per sample δεν αλλάζει  */

    if (bitspersample != 8 && bitspersample!= 16)  {
    fprintf(stderr, "Error! bits per sample should be 8 or 16\n");
    exit(1);
    }

    /* "data" chunk  */
    int data[4];

    /* διαβάζουμε τα 4 bytes του "data" από το input */
    for (i = 0; i < 4; i++) {
        data[i] = rbyte();
    }

    /* έλεγχος ότι είναι 'd','a','t','a' */
    if (data[0] != 'd' || data[1] != 'a' || data[2] != 't' || data[3] != 'a') {
        fprintf(stderr, "Error! \"data\" not found\n");
        exit(1);
    }

   wbyte('d');/* γράφουμε το "data" στο output */
    wbyte('a');
    wbyte('t');
    wbyte('a');

    /* data size */
    unsigned int datasize = b4(); 
    unsigned int datasize_output;

    if (channels == 1) {
        
        datasize_output = datasize;/* mono input τότε κρατάμε όλα τα samples και άρα ίδιο datasize */
    } 
    else {
        /* αν έχουμε stereo input τότε κρατάμε μόνο ένα κανάλι( αφού το κάναμε mono) και άρα τα bytes στο νέο αρχείο θα είναι τα μισά*/
        datasize_output = datasize / 2;
    }
    w4(datasize_output); /* γράφουμε το νέο datasize στο output */

    if (channels == 1) {
        /*στο νέο αρχείο θα μεταφερθούν τα samples όπως είναι*/
        unsigned int k;
        for (k = 0; k < datasize; k++) {
            int c = rbyte();   /* διαβάζουμε 1 byte από τα δεδομένα ήχου */
            wbyte(c);          /* το γράφουμε όπως είναι στην έξοδο */
        }
    }
    
    else {  /* απο stereo input σε mono output */
        int  unsigned k;
        if(bitspersample==8){
            for(k=0;k<datasize;k++){
                int c = rbyte();
                if((strcmp(mode, "left") == 0)&&(k%2==0)){/*τα left samples βρίσκονται σε ζυγές θέσεις , τα right samples σε περιττές*/
                     wbyte(c);
                }
                else if ((strcmp(mode, "right") == 0)&&(k%2!=0)){   
                    wbyte(c);
                 }
            }
        }
        else if(bitspersample==16){
            int temp=0;
            for(k=0;k<datasize;k+=2){
                temp++;/*αυτό δείχνει αν βρισκόμαστε σε σημείο που υπάρχουν left samples ή right samples*/
                int c1 = rbyte();/*εδώ αποθηκεύονται τα δύο bytes αφού για bitspersample=16 υπάρχουν 2 bytes για κάθε sample*/
                int c2 = rbyte();
                if((strcmp(mode, "left") == 0)&&(temp%2!=0)){/*εδώ για temp ίσο με περιττη τιμή βρισκόμαστε σε left samples και για ζυγή τιμή σε right samples*/
                     wbyte(c1);
                     wbyte(c2);
                }
                else if ((strcmp(mode, "right") == 0)&&(temp%2==0)){   
                    wbyte(c1);
                    wbyte(c2);
                 }
            }
        }
    }
    unsigned int j;
    unsigned int extradata=filesize-36-datasize;/*τα εξτρα δεδομένα τα βρισκουμε αν αφαιρέσουμε απο όλο το filesize τα 36 bytes του header και τα sample data*/

    for(j=0;j<extradata;j++){
        int c = getchar();   // διάβασμα ενός byte κάθε φορά που εκτελείται η for απο το αρχείο*/ 
        if (c == EOF) { /*αν φτάσει σε EOF πριν να διαβάσει τον αριθμό των bytes που πρέπει με βάση το size που διαβάσαμε πριν*/
        fprintf(stderr, "Error! insufficient data\n");/*το πρόγραμμα τερματίζεται με σφάλμα αφού το αρχείο είναι ελλειπής*/ 
        exit(1);
        }
        putchar(c);
    }
    int extra = getchar();/* κανονικά αυτη η μεταβλητή πρέπει αντιστοιχεί με EOF*/
    if (extra != EOF  ) { /* αν δεν αντιστοιχεί με EOF τότε σημαίνει πως υπάρχουν περισσότερα δεδομένα στο αρχείο αποτι θα έπρεπε και σε αυτή την περίπτωση το προγραμμα τερματίζεται με σφάλμα*/
    fprintf(stderr, "Error! bad file size (found data past the expected end of file)\n");
    exit(1); 
    }


}


int main(int argc, char *argv[]) {  /*argv είναι ένας πίνακας από char pointers*/

    if (argc == 1) {/*σε περίπτωση που ο χρήστης δώσει μόνο ένα όρισμα και καμία υποεντολή δεν μπορεί να εκτελεστεί*/
    fprintf(stderr, "Error! No command given\n");/*το πρόγραμμα τερματίζεται με σφάλμα*/
    exit(1);
}
    /*ελέγχουμε αν ο χρήστης θέλει να εκτελεστεί η υποεντολή info */
    /* Πρέπει να υπάρχουν ακριβώς 2 ορίσματα:
       1) το όνομα του προγράμματος (argv[0])
       2) η υποεντολή "info"*/
    
    /* Ελέγχουμε ότι η εντολή είναι "info" ώστε να καλέσουμε την συνάρτηση */
    if (strcmp(argv[1], "info") == 0) { /* η συγκεκριμένη συνάρτηση συγκρίνει αν τα δύο strings που έχω στην συνθήκη είναι ακριβώς τα ίδια */
        if (argc != 2) {
        fprintf(stderr, "Error! Wrong number of arguments\n");
        exit(1);
        }
        info(); /*αν επιστρέψει 0 (δηλ. είναι τα ίδια ) επιστρέφει 0 και καλούμε τη συναρτηση info */
    }
    
    /* εδώ ελέγχουμε αν ο χρήστης θέλει να εκτελέσει στην υποεντολή rate */
    /* πρέπει να υπάρχουν ακριβώς 3 ορίσματα:
           1) ./soundwave
           2) rate
           3) factor (δηλαδή τον παράγοντα που θα μεταβάλει το αρχείο 
    */
    else if (strcmp(argv[1], "rate") == 0) {

         if (argc != 3) {
            fprintf(stderr, "Error! Wrong number of arguments for rate\n");
            exit(1);
        }

        
        char *end;/* μετατροπή του factor από string σε double */
        double factor = strtod(argv[2], &end);

        
        if (*end != '\0') {/* έλεγχος ότι το argv[2] ήταν όντως αριθμός */
            fprintf(stderr, "Error! invalid rate factor \"%s\"\n", argv[2]);
            exit(1);
        }

        rate(factor);
    }
    /* εδώ ελέγχουμε αν ο χρήστης θέλει να εκτελέσει στην υποεντολή channel*/
    /* πρέπει να υπάρχουν ακριβώς 3 ορίσματα:
           1) ./soundwave
           2) channel
           3) mode (απο ποιο κανάλι θα κρατήσουμε τα samples)
    */
    else if (strcmp(argv[1], "channel") == 0) {
        
    if (argc != 3) {
        fprintf(stderr, "Error! Wrong number of arguments for channel\n");
        exit(1);
    }

    /* Το τρίτο όρισμα πρέπει να είναι "left" ή "right" */
    if (strcmp(argv[2], "left") != 0 && strcmp(argv[2], "right") != 0) {
        fprintf(stderr, "Error! channel mode must be \"left\" or \"right\"\n");
        exit(1);
    }

    /* Αν όλα είναι σωστά εκτελούμε την channel() */
    channel(argv[2]);
    }

else {
    fprintf(stderr, "Error! Unknown command \"%s\"\n", argv[1]);
    exit(1);
}
    return 0;
}
