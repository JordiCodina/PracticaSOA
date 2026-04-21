//hack per passar de Unix Time a una cadena.
char * unixTimeToStringHack (time_t timeInt){

    struct tm timestamp; //estructura timestamp de la llibreria <time.h>
    localtime_r(&timeInt, &timestamp); //funcio de <time.h>. de int a timestamp
    char *date = asctime(&timestamp); //funcio de <time.h>. de timestamp a cadema

    return date;
}
//generear el mateix pero amb '\0' al final.
char *generateStringWithZeroAtEnd (unsigned char str[], unsigned long strSize){
    char *buf = malloc (strSize + 1);
    memcpy(buf, str, strSize);
    buf[strSize] = '\0';

    return buf;
}