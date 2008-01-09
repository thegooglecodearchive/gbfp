#define __EXTENSIONS__

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
 
#include "gbfp.h"
 
const char sVer[] = "0.3.0";
const char sNorBase[] = "ACGTRYMKWSBDHVNacgtrymkwsbdhvn";
const char sComBase[] = "TGCAYRKMWSVHDBNtgcayrkmwsvhdbn";
const unsigned int iBaseLen = 30;

static void rtrim(char *sLine) {
    register int i;
    
    for (i = (strlen(sLine) - 1); i >= 0; i--) {
        if (! isspace(sLine[i])) {
            sLine[++i] = '\0';
            break;
        }
    }
}

static void rtrim2(char *sLine, char cRemove) {
    register int i;
    
    for (i = (strlen(sLine) - 1); i >= 0; i--) {
        if (sLine[i] == cRemove) {
            sLine[i] = '\0';
            break;
        }
    }
}

static int Pos2Num(char *psPositions, int aiPositions[]) {
    register int i;
    int iNum = 0;

    for (i = strlen(psPositions); i >= 0; i--) {
        if (isdigit(*(psPositions + i))) aiPositions[(aiPositions[iNum] - 1 == i) ? iNum : ++iNum] = i;
        else *(psPositions + i) = '\0';
    }

    return iNum;
}

static int Positions2Numbers(char *psPositions, unsigned long *lStart, unsigned long *lEnd) {
    int aiPositions[16] = {-2,};
    int iNum;

    /*
    printf("%s\t", psPositions);
    */

    iNum = Pos2Num(psPositions, aiPositions);
   
    if (iNum == 2) {
        *lStart = atol(psPositions + aiPositions[2]);
        
        *lEnd = atol(psPositions + aiPositions[1]);

        /*
        printf("%li\t%li\n", *lStart, *lEnd);
        */

        return 1;
    } else if (iNum == 1) {
        *lStart = *lEnd = atol(psPositions + aiPositions[1]);
       
        /* 
        printf("%li\t%li\n", *lStart, *lEnd);
        */

        return 1;
    } else {
        fprintf(stderr, "Warning: cannot parse '%s'\n", psPositions);

        return 0;
    }

}

static void LocusParser(char *sLocusStr, struct tGBFFData *ptGBFFData) {
    /*    
    01-05      'LOCUS'
    06-12      spaces
    13-28      Locus name
    29-29      space
    30-40      Length of sequence, right-justified
    41-41      space
    42-43      bp
    44-44      space
    45-47      spaces, ss- (single-stranded), ds- (double-stranded), or
               ms- (mixed-stranded)
    48-53      NA, DNA, RNA, tRNA (transfer RNA), rRNA (ribosomal RNA),
               mRNA (messenger RNA), uRNA (small nuclear RNA), snRNA,
               snoRNA. Left justified.
    54-55      space
    56-63      'linear' followed by two spaces, or 'circular'
    64-64      space
    65-67      The division code (see Section 3.3)
    68-68      space
    69-79      Date, in the form dd-MMM-yyyy (e.g., 15-MAR-1991)
    */
    
    regex_t ptRegExLocus;
    regmatch_t ptRegMatch[7];
    
    const char sLocus[] = "LOCUS +([a-z|A-Z|0-9|_]+) +([0-9]+) bp +([a-z|A-Z|-]+) +([a-z]+) +([A-Z]{3}) (.+)";
    char sTemp[LINELEN];
    unsigned int i, iLen;
    
    
    struct tData {
        char cType;
        void *Pointer;
    };
    
    struct tData tDatas[] = {
        {STRING, NULL},
        {LONG, NULL},
        {STRING, NULL},
        {STRING, NULL},
        {STRING, NULL},
        {STRING, NULL}};

    tDatas[0].Pointer = ptGBFFData->sLocusName;
    tDatas[1].Pointer = &(ptGBFFData->lLength);
    tDatas[2].Pointer = ptGBFFData->sType;
    tDatas[3].Pointer = ptGBFFData->sTopology;
    tDatas[4].Pointer = ptGBFFData->sDivisionCode;
    tDatas[5].Pointer = ptGBFFData->sDate;

    i = regcomp(&ptRegExLocus, sLocus, REG_EXTENDED | REG_ICASE);
    if (i != 0) {
        regerror(i, &ptRegExLocus, sTemp, LINELEN);
        fprintf(stderr, "%s\n", sTemp);
        exit(1);
    }
    
    rtrim(sLocusStr);
        
    if (regexec(&ptRegExLocus, sLocusStr, 7, ptRegMatch, 0) == 0) {
        for (i = 0; i < 6; i++) {
            iLen = ptRegMatch[i + 1].rm_eo - ptRegMatch[i + 1].rm_so;
            switch (tDatas[i].cType) {
            case STRING:
                memcpy(tDatas[i].Pointer, (sLocusStr + ptRegMatch[i + 1].rm_so), iLen);
                *((char *) tDatas[i].Pointer + iLen) = '\0';
                /*
                printf("%i, %s\n", iLen, (char *) tDatas[i].Pointer);
                */
                break;
            case LONG:
                memcpy(sTemp, (sLocusStr + ptRegMatch[i + 1].rm_so), iLen);
                sTemp[iLen] = '\0';
                *((unsigned long *) tDatas[i].Pointer) = atol(sTemp);
                break;
            default:
                perror("Unknown Data Type!!!");
            }
        }
    } else {
        regerror(i, &ptRegExLocus, sTemp, LINELEN);
        fprintf(stderr, "%s\n", sTemp);
        exit(1);
    }
}

static char *checkComplement(char *sLocation) {
    char *sPosition;

    for (;isspace(*sLocation);sLocation++);

    for (sPosition = sLocation; *sPosition; sPosition++) {
        /* Check the 1st and the 2nd characters of 'complement' */
        if (*sPosition == 'c' && *(sPosition + 1) == 'o') {
            rtrim2(sLocation, ')');
            return sPosition + 11;
        }
    }

    return sLocation;
}

static char *checkJoin(char *sLocation) {
    char *sPosition;

    for (;isspace(*sLocation);sLocation++);

    for (sPosition = sLocation; *sPosition; sPosition++) {
        /* Check the 1st and the 2nd characters of 'complement' */
        if (*sPosition == 'j' && *(sPosition + 1) == 'o') {
            rtrim2(sLocation, ')');
            return sPosition + 5;
        }
    }

    return sLocation;
}

/* Parsing a string which contains location information */

static void LocationParser(char *sLocation, struct tFeature *pFeature) {
    char *psTemp;
    char *psString = NULL;
    char *psLocation = NULL;
    
    unsigned int iLocationNum = 1;
    
    /* Evalue sequence direction
    psString has location and join informations
    */

    psString = checkComplement(sLocation);

    if (sLocation == psString) pFeature->cDirection = NORMAL;
    else pFeature->cDirection = REVCOM;

    /* Remove 'join' string
    psString has location informations
    */

    psString = checkJoin(psString);

    psTemp = psString - 1;
    while((psTemp = strchr((psTemp + 1), ','))) iLocationNum++;
    pFeature->ptLocation = malloc(iLocationNum * sizeof(*(pFeature->ptLocation)));

    iLocationNum = 0;
    psLocation = strtok_r(psString, ",", &psTemp);
    if (Positions2Numbers(psLocation,
        &(((pFeature->ptLocation)+iLocationNum)->lStart),
        &(((pFeature->ptLocation)+iLocationNum)->lEnd)) == 1) iLocationNum++;

    while((psLocation = strtok_r(NULL, ",", &psTemp))) {
        if (Positions2Numbers(psLocation,
            &(((pFeature->ptLocation)+iLocationNum)->lStart),
            &(((pFeature->ptLocation)+iLocationNum)->lEnd))) iLocationNum++;
            
    }
    
    pFeature->lStart = (pFeature->ptLocation)->lStart;
    pFeature->lEnd = ((pFeature->ptLocation)+(iLocationNum - 1))->lEnd;
    pFeature->iLocationNum = iLocationNum;
}

static char *parseQualifier(char *psQualifier, char **psValue) {
    char *sPosition;

    for (;isspace(*psQualifier); psQualifier++);

    if ((sPosition = strchr(psQualifier, '=')) == NULL) return sPosition;

    *sPosition = '\0';

    sPosition++;

    for (;isspace(*psQualifier); psQualifier++);

    if (*sPosition == '"') {
        sPosition++;
        rtrim2(sPosition, '"');
    }

    *psValue = sPosition;

    return psQualifier;
}

static void QualifierParser(char *psQualifier, struct tFeature *pFeature) {
    char *sValue;
    char *psTemp = NULL;
    char *psString = NULL;
    struct tQualifier *ptQualifier;
    
    /* printf("\n\n%s\n", psQualifier); */
    
    pFeature->ptQualifier = malloc(INITQUALIFIERNUM * sizeof(struct tQualifier));
    ptQualifier = pFeature->ptQualifier;

    /* Parse the 1st qualifier string */
    psString = strtok_r(psQualifier, "\n", &psTemp);

    /* printf("%s\n", psString); */

    if ((psQualifier = parseQualifier(psString, &sValue)) != NULL) {
        ptQualifier->psQualifier = psQualifier;
        ptQualifier->psValue = sValue;
    }
    /* printf("%s ----- %s\n", ptQualifier->psQualifier, ptQualifier->psValue); */
    ptQualifier++;
    
    /* Parse the rest qualifier string */
    while((psString = strtok_r(NULL, "\n", &psTemp))) {
        /* printf("%s\n", psString); */
        
        if ((psQualifier = parseQualifier(psString, &sValue)) != NULL) {
            ptQualifier->psQualifier = psQualifier;
            ptQualifier->psValue = sValue;
        }
        /* printf("%s ----- %s\n", ptQualifier->psQualifier, ptQualifier->psValue); */
        ptQualifier++;
    }

    pFeature->iQualifierNum = ptQualifier - pFeature->ptQualifier;
    pFeature->ptQualifier =  realloc(pFeature->ptQualifier, pFeature->iQualifierNum * sizeof(struct tQualifier));
}

static unsigned int SequenceParser(char *psSequence, char *psSequence2) {
    register unsigned int i = 0;
    register unsigned int j = 0;
    register char c;
    
    while((c = *(psSequence + i++)) != '\0')
        if (isalpha(c) != 0) *(psSequence2 + j++) = c;

    *(psSequence2 + j) = '\0';
        
    return j;
}

static void RevCom(char *psSequence) {
    char c;
    unsigned int k;
    unsigned long i, j;
    
    for (i = 0, j = strlen(psSequence) - 1; i < j; i++, j--) {
	c = *(psSequence + i);
	*(psSequence + i) = 'X';
	for (k = 0; k < iBaseLen; k++)
	    if (*(sNorBase + k) == *(psSequence + j)) {
		*(psSequence + i) = *(sComBase + k);
		break;
	    }
	*(psSequence + j) = 'X';
	for (k = 0; k < iBaseLen; k++)
	    if (*(sNorBase + k) == c) {
		*(psSequence + j) = *(sComBase + k);
		break;
	}
    }
}

char *GBFF_Get_Sequence(char *psSequence, struct tFeature *ptFeature) {
    unsigned long lSeqLen = 1; /* For the '\0' characher */
    unsigned long lStart, lEnd;
    unsigned int i;
    char *psSequenceTemp;
    
    for (i = 0; i < ptFeature->iLocationNum; i++)
        lSeqLen += (((ptFeature->ptLocation) + i)->lEnd - ((ptFeature->ptLocation) + i)->lStart + 1);
    
    psSequenceTemp = malloc(lSeqLen * sizeof(char));
    
    lSeqLen = 0;
    
    for (i = 0; i < ptFeature->iLocationNum; i++) {
        lStart = ((ptFeature->ptLocation) + i)->lStart;
        lEnd = ((ptFeature->ptLocation) + i)->lEnd;
        strncpy(psSequenceTemp + lSeqLen, psSequence + lStart - 1, lEnd - lStart + 1);
        lSeqLen += (lEnd - lStart + 1);
    }
    
    *(psSequenceTemp + lSeqLen) = '\0';
    
    if (ptFeature->cDirection == REVCOM) RevCom(psSequenceTemp);

    return psSequenceTemp;
}

void GBFF_Free(struct tGBFFData **pptGBFFData) {
    struct tGBFFData *ptGBFFData = NULL;
    struct tFeature *tpFeatures = NULL;
    unsigned int iFeatureNumber = 0;
    unsigned int iFeatureNum = 0;
    unsigned int iSeqPos = 0;
  
    for (iSeqPos = 0; *(pptGBFFData + iSeqPos) != NULL; iSeqPos++) {
        ptGBFFData = *(pptGBFFData + iSeqPos);

        tpFeatures = ptGBFFData->ptFeatures;
        iFeatureNumber = ptGBFFData->iFeatureNumber;
    
        for (iFeatureNum = 0; iFeatureNum < iFeatureNumber; iFeatureNum++) {
            /*
            printf("%i, %i\n", iFeatureNum, (tpFeatures+iFeatureNum)->iQualifierNum);
            */
            free((tpFeatures+iFeatureNum)->ptLocation);
            free(((tpFeatures+iFeatureNum)->ptQualifier)->psQualifier);
        }
    }

    free(tpFeatures);
}

static struct tGBFFData *_GBFF_Parser(FILE *FSeqFile) {
    char sLine[LINELEN] = {'\0',};
    char sLocation[LINELEN] = {'\0',};
    char *psQualifier = NULL;
    char *psQualifierTemp = NULL;
   
    unsigned int iReadPos = INELSE;
    unsigned int iFeatureNum = 0;
    unsigned int iFeatureMem = INITFEATURENUM;
    unsigned int i = 0;
    unsigned long lSeqLen;
    
    struct tFeature *pFeatures = NULL;
    struct tFeature *pFeature = NULL;
    struct tGBFFData *ptGBFFData = NULL;
    
    pFeatures = (struct tFeature *) malloc(iFeatureMem * sizeof(struct tFeature));

    /* Confirming GBFF File with LOCUS line */
    while(fgets(sLine, LINELEN, FSeqFile)) {
        if (memcmp(sLine, "LOCUS", 5) == 0) {
            ptGBFFData = malloc(sizeof(struct tGBFFData));
            break;
        }
    }

    /* If there is a no LOCUS line, next statement return NULL value to end parsing */
    if (ptGBFFData == NULL) return NULL;
   
    /* Parse LOCUS line */ 
    LocusParser(sLine, ptGBFFData);

    while(fgets(sLine, LINELEN, FSeqFile))
        if (memcmp(sLine, "FEATURES", 8) == 0) break;

    /* Parse FEATURES */
    while(fgets(sLine, LINELEN, FSeqFile)) {
        rtrim(sLine);

        if (memcmp(sLine, "BASE COUNT", 10) == 0 || memcmp(sLine, "ORIGIN", 6) == 0) {
            if (iFeatureNum == iFeatureMem) {
                iFeatureMem += INITFEATURENUM;
                pFeatures = realloc(pFeatures, sizeof(struct tFeature) * iFeatureMem);
            }

            if (strlen(sLocation) != 0) LocationParser(sLocation, (pFeatures + iFeatureNum - 1));
            if (psQualifier < psQualifierTemp) {
                *psQualifierTemp++ = '\n';
                *psQualifierTemp = '\0';
                psQualifier = realloc(psQualifier, (psQualifierTemp - psQualifier + 1) * sizeof(*psQualifier));
                QualifierParser(psQualifier, (pFeatures + iFeatureNum - 1));
            }
            
            break;
        }
        
        if (memcmp(sLine + 5, "               ", 15) != 0) {
            if (iFeatureNum == iFeatureMem) {
                iFeatureMem += INITFEATURENUM;
                pFeatures = realloc(pFeatures, sizeof(struct tFeature) * iFeatureMem);
            }

            if (strlen(sLocation) != 0) LocationParser(sLocation, (pFeatures + iFeatureNum - 1));
            if (psQualifier < psQualifierTemp) {
                *psQualifierTemp++ = '\n';
                *psQualifierTemp = '\0';
                /*
                printf("=====\n%s=====", psQualifier);
                */
                psQualifier = realloc(psQualifier, (psQualifierTemp - psQualifier + 1) * sizeof(*psQualifier));
                QualifierParser(psQualifier, (pFeatures + iFeatureNum - 1));
            }

            *sLocation = '\0';
            psQualifier = malloc(sizeof(*psQualifier) * LINELEN);
            psQualifierTemp = psQualifier;
            
            iReadPos = INFEATURE;
            
            memcpy((pFeatures + iFeatureNum)->sFeature, (sLine + 5), 15);
            *(((pFeatures + iFeatureNum)->sFeature) + 15) = '\0';
            rtrim((pFeatures + iFeatureNum)->sFeature);
            strcpy(sLocation, (sLine + 21));

            /* Feature Initalize */
            pFeature = pFeatures + iFeatureNum;
            pFeature->iNumber = iFeatureNum;
            pFeature->cDirection = NORMAL;
            pFeature->iLocationNum = 0;
            pFeature->lStart = 0;
            pFeature->lEnd = 0;
            pFeature->iQualifierNum = 0;
            pFeature->ptLocation = NULL;
            pFeature->ptQualifier = NULL;

            iFeatureNum++;
        } else if (*(sLine + QUALIFIERSTART) == '/') {
            iReadPos = INQUALIFIER;
            if (psQualifier < psQualifierTemp) *psQualifierTemp++ = '\n';
            i = strlen(sLine) - (QUALIFIERSTART + 1);
            memcpy(psQualifierTemp, sLine + (QUALIFIERSTART + 1), i);
            psQualifierTemp += i;
        } else {
            if (iReadPos == INFEATURE) {
                strcpy((sLocation + strlen(sLocation)), (sLine + QUALIFIERSTART));
            } else if (iReadPos == INQUALIFIER) {
                i = strlen(sLine) - QUALIFIERSTART;
                memcpy(psQualifierTemp, sLine + QUALIFIERSTART, i);
                psQualifierTemp += i;
            }
        }
    }
    
    if (memcmp(sLine, "ORIGIN", 6) != 0)
        while(fgets(sLine, LINELEN, FSeqFile))
            if (memcmp(sLine, "ORIGIN", 6) == 0) break;

    lSeqLen = 0;
    ptGBFFData->psSequence = malloc((ptGBFFData->lLength + 1) * sizeof(char));
    
    while(fgets(sLine, LINELEN, FSeqFile)) {
        if (memcmp(sLine, "//", 2) == 0) break;

        lSeqLen += SequenceParser(sLine, ptGBFFData->psSequence + lSeqLen);
    }

    ptGBFFData->iFeatureNumber = iFeatureNum;
    ptGBFFData->ptFeatures = pFeatures;
    
    return ptGBFFData;
}

struct tGBFFData **GBFF_Parser(char *spFileName) {
    int iGBFSeqPos = 0;
    unsigned int iGBFSeqNum = INITGBFSEQNUM;
    struct tGBFFData **pptGBFFDatas;
    FILE *FSeqFile;
 
    if (spFileName == NULL) {
        FSeqFile = stdin;
    } else {
        if (access(spFileName, F_OK) != 0) {
            /* perror(spFileName); */
            return NULL;
        } else {
            FSeqFile = fopen(spFileName, "r");
        }
    }
    
    pptGBFFDatas = malloc(iGBFSeqNum * sizeof(struct tGBFFData *));

    do {
        if (iGBFSeqNum == iGBFSeqPos) {
            iGBFSeqNum += INITGBFSEQNUM;
            pptGBFFDatas = realloc(pptGBFFDatas, iGBFSeqNum * sizeof(struct tGBFFData *));
        }
        *(pptGBFFDatas + iGBFSeqPos) = _GBFF_Parser(FSeqFile);
    } while (*(pptGBFFDatas + iGBFSeqPos++));
    
    if (spFileName) fclose(FSeqFile);

    return pptGBFFDatas;
}