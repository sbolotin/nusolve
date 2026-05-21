#include <stdlib.h>
#include <iostream>
#include <cmath>

#include <QtCore/QString>


#include <SgMJD.h>

#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;

#ifdef HAVE_MK4_DATA_H
extern "C"
{
// workaround "complex" stuff:
  #define T230_VERSION 0
  #include <hops/mk4_data.h>
}
#endif
#ifdef HAVE_MK4_DFIO_H
extern "C"
{
  #include <hops/mk4_dfio.h>
}
#endif
#ifdef HAVE_MK4_VEX_H
// workaround warnings about redefinitions of TRUE and FALSE (Qt vs HOPS):
# ifdef FALSE
#   define FALSE_SAVED FALSE
#   undef FALSE
# endif
# ifdef TRUE
#   define TRUE_SAVED TRUE
#   undef TRUE
# endif
extern "C"
{
    #include <hops/mk4_vex.h>
}
# ifdef FALSE_SAVED
#   undef FALSE
#   define FALSE FALSE_SAVED
#   undef FALSE_SAVED
# endif
# ifdef TRUE_SAVED
#   undef TRUE
#   define TRUE TRUE_SAVED
#   undef TRUE_SAVED
# endif
#endif


//
#if !defined HAVE_MK4_DATA_H || !defined HAVE_MK4_DFIO_H || !defined HAVE_MK4_VEX_H
int main(int, char **)
{
 std::cout << "No HOPS detected.\n";
 return 0;
};
//
//
#else


#ifdef OLD_HOPS
char progname[] = "FRN_dump";
int msglev = 2;
#endif


void pr_date_old (const char *string, date date)
{
 printf ("%s: %04d-%03d %02d:%02d:%11.8f\n",
    string, date.year, date.day, date.hour, date.minute, date.second);
 fflush (stdout);
};

void pr_date (const char *prefix, date d)
{
 SgMJD                          epoch;
 epoch.setUpEpoch(d.year, 0, d.day, d.hour, d.minute, d.second);
 std::cout << prefix << ": " << qPrintable(epoch.toString()) << "\n";
};

void pr_hdr (const char *prefix, char *id, char *ver)
{
 printf ("++++ %s structure [id=%c%c%c, ver=%c%c]:\n",
    prefix,
    *id, *(id+1), *(id+2),
    *ver, *(ver+1));
};




//
//
//
//
int main(int argc, char **argv)
{
 QString              fileName("");
 QString                   str("");

 if (argc>1)
 {
    fileName = argv[1];
 }
 else
 {
    std::cout << "say a file name\n";
    return 1;
 };
 char        *fn;
 fn = new char[fileName.size() + 1];
 strncpy(fn, qPrintable(fileName), fileName.size()+1);

 std::cout << "Trying to read [" << fn << "] file\n";

 mk4_fringe       fringe;

// mk4_sdata sdata;
 type_200 *t200;
 type_201 *t201;
 type_202 *t202;
 type_203 *t203;
 type_204 *t204;
 type_205 *t205;
 type_206 *t206;
 type_207 *t207;
 type_208 *t208;
 type_210 *t210;
// type_212 *t212;

 int                            num;
 fringe.nalloc = 0;

 if (read_mk4fringe(fn, &fringe) != 0)
 {
    std::cout << "Error return from read_mk4fring\n";
    exit (1);
 };
 int numOfChan=0;

 t200 = fringe.t200;
 t201 = fringe.t201;
 t202 = fringe.t202;
 t203 = fringe.t203;
 t204 = fringe.t204;
 t205 = fringe.t205;
 t206 = fringe.t206;
 t207 = fringe.t207;
 t208 = fringe.t208;
 t210 = fringe.t210;
// t212 = fringe.t212[0];


 printf ("\n");
 printf ("fringe.n212 = %d\n", fringe.n212);
 printf ("fringe.n230 = %d\n", fringe.n230);
 printf ("\n");


// t200:
 pr_hdr ("type 200 (general information)", t200->record_id, t200->version_no);
 printf ("t200 software_rev =");
 for (int i=0; i<10; i++)
    printf (" %d", t200->software_rev[i]);
 printf ("\n");
 printf ("t200 Experiment '%s', No %d. Correlator '%s'\n", t200->exper_name, t200->expt_no, t200->correlator);
 printf ("t200 Scan name '%s', Start/Stop offsets: %d/%d\n", t200->scan_name, t200->start_offset, t200->stop_offset);
 pr_date ("t200 scantime    ", t200->scantime);
 pr_date ("t200 corr_date   ", t200->corr_date);
 pr_date ("t200 fourfit_date", t200->fourfit_date);
 pr_date ("t200 frt         ", t200->frt);
 printf ("----\n");
 printf ("\n");


//t201:
 pr_hdr ("type 201 (source information)", t201->record_id, t201->version_no);
 printf ("t201 source = '%s' RA= %2dh %02dm %gs De=%2d:%02d:%g on the epoch of %d\n",
    t201->source, 
    t201->coord.ra_hrs, t201->coord.ra_mins, t201->coord.ra_secs,
    t201->coord.dec_degs, t201->coord.dec_mins, t201->coord.dec_secs,
    t201->epoch);
 pr_date ("t201 coord_date", t201->coord_date);
 printf ("t201 rates: RA= %g De= %g\n", t201->ra_rate, t201->dec_rate);
 printf ("t201 pulsar phase = [%g][%g][%g][%g] on epoch %g with dispersion %g\n", 
    t201->pulsar_phase[0], t201->pulsar_phase[1], t201->pulsar_phase[2], t201->pulsar_phase[3],
    t201->pulsar_epoch, t201->dispersion);
 printf ("----\n");
 printf ("\n");


//t202:
 pr_hdr ("type 202 (baseline information)", t202->record_id, t202->version_no);
 printf ("t202 baseline: '%.2s'; ref: '%.2s'[%.8s], rem: '%.2s'[%.8s]\n",
    t202->baseline,
    t202->ref_intl_id, t202->ref_name,
    t202->rem_intl_id, t202->rem_name);
 printf ("t202 tape ref: '%.8s', rem: '%.8s'; nlags=%d\n",
    t202->ref_tape, t202->rem_tape, t202->nlags);
 printf ("t202 coords ref= (%12.3f, %12.3f, %12.3f), rem= (%12.3f, %12.3f, %12.3f)\n",
    t202->ref_xpos, t202->ref_ypos, t202->ref_zpos,
    t202->rem_xpos, t202->rem_ypos, t202->rem_zpos);
 printf ("t202 clocks ref= %g, %g   rem= %g, %g\n",
    t202->ref_clock, t202->ref_clockrate,
    t202->rem_clock, t202->rem_clockrate);
 printf ("t202 idelay ref= %g, rem= %g; zdelay ref= %g, rem= %g\n",
    t202->ref_idelay, t202->rem_idelay,
    t202->ref_zdelay, t202->rem_zdelay);
 printf ("t202 pointing(el,az) ref= (%g, %g), rem= (%g, %g)\n",
    t202->ref_elev, t202->ref_az,
    t202->rem_elev, t202->rem_az);
 printf ("t202 fringes (u,v)=(%g, %g), (uf, vf)=(%g, %g)\n",
    t202->u, t202->v, t202->uf, t202->vf);
 printf ("----\n");
 printf ("\n");


//t203:
 num = 32;
#ifdef MAX_CHAN_PP
 if (t203->version_no[0]=='0' && t203->version_no[1]=='1')
    num = 8*MAX_CHAN_PP;
#endif
#ifdef MAXFREQ
 if (t203->version_no[0]=='0' && t203->version_no[1]=='1')
    num = 8*MAXFREQ;
#endif
 pr_hdr ("type 203 (channel information)", t203->record_id, t203->version_no);
 for (int i=0; i<num; i++)
    if (t203->channels[i].index != -1)
        printf ("t203 channel[%3d]:  index=%3d samplrate=%d SideBands:'%c%c' Pols:'%c%c' Frqs=(%g,%g) ChanIds: '%.8s' '%.8s'\n",
                    i,
                    t203->channels[i].index,
                    t203->channels[i].sample_rate,
                    t203->channels[i].refsb,
                    t203->channels[i].remsb,
                    t203->channels[i].refpol,
                    t203->channels[i].rempol,
                    t203->channels[i].ref_freq,
                    t203->channels[i].rem_freq,
                    t203->channels[i].ref_chan_id,
                    t203->channels[i].rem_chan_id);
 printf ("----\n");
 printf ("\n");


//t204:
 pr_hdr ("type 204 (execution setup)", t204->record_id, t204->version_no);
 printf ("t204 ff_version= (%d,%d) on the platform '%.8s'; control file: '%s' of the epoch",
    t204->ff_version[0], t204->ff_version[1],
    t204->platform, t204->control_file);
 pr_date ("", t204->ffcf_date);
 printf ("t204 override = '%s'\n", t204->override);
 printf ("----\n");
 printf ("\n");


//t205:
 num = 16;
 if (t205->version_no[0]=='0' && t205->version_no[1]=='1')
    num = 64;
 pr_hdr ("type 205 (fourfit setup)", t205->record_id, t205->version_no);
 printf  ("t205 search= [%g:%g][%g:%g][%g:%g]; filter= [%g][%g][%g][%g][%g][%g][%g][%g]\n",
    t205->search[0], t205->search[1],
    t205->search[2], t205->search[3],
    t205->search[4], t205->search[5],
    t205->filter[0], t205->filter[1], t205->filter[2], t205->filter[3], t205->filter[4], t205->filter[5], t205->filter[6], t205->filter[7]);
 printf  ("t205 offset= %g, ffmode: '%.8s', ref_freq= %g\n",
    t205->offset, t205->ffmode, t205->ref_freq);
 pr_date ("t205 utc_central: ", t205->utc_central);
 pr_date ("t205 start:       ", t205->start);
 pr_date ("t205 stop:        ", t205->stop);
 for (int i=0; i<num; i++)
    if (t205->ffit_chan[i].channels[0]!=-1 ||
        t205->ffit_chan[i].channels[1]!=-1  )
    {
  printf ("t205 ffit_chan_id[%3d]: (%c) idxs: %2d:%2d:%2d:%2d\n", i,
                    t205->ffit_chan[i].ffit_chan_id,
                    t205->ffit_chan[i].channels[0],
                    t205->ffit_chan[i].channels[1],
                    t205->ffit_chan[i].channels[2],
                    t205->ffit_chan[i].channels[3]
                    );
  numOfChan++;
    };
 printf ("----\n");
 printf ("\n");


////t206:
//
// num = 16;
// if (t205->version_no[0]=='0' && t205->version_no[1]=='2')
//    num = 64;
 pr_hdr ("type 206 (data filtering)", t206->record_id, t206->version_no);
 pr_date ("t206 start: ", t206->start);
 printf  ("t206 first/last AP= (%d,%d); integration time= %g, %% of accept/discard= (%g, %g)\n",
    t206->first_ap, t206->last_ap,
    t206->intg_time, t206->accept_ratio, t206->discard);
 printf  ("t206 ratesize, mbdsize, sbdsize: %d, %d, %d\n", t206->ratesize, t206->mbdsize, t206->sbdsize);
 for (int i=0; i<numOfChan; i++)
    printf ("t206 chan[%2d]: accepted=(l:%4d u:%4d) weights=(l:%12g u:%12g) 8 reasons(l:u)=(%d:%d)(%d:%d)(%d:%d)(%d:%d)(%d:%d)(%d:%d)(%d:%d)(%d:%d)\n",
        i,
        t206->accepted[i].lsb,  t206->accepted[i].usb,
        t206->weights[i].lsb,   t206->weights[i].usb,
        t206->reason1[i].lsb,   t206->reason1[i].usb,
        t206->reason2[i].lsb,   t206->reason2[i].usb,
        t206->reason3[i].lsb,   t206->reason3[i].usb,
        t206->reason4[i].lsb,   t206->reason4[i].usb,
        t206->reason5[i].lsb,   t206->reason5[i].usb,
        t206->reason6[i].lsb,   t206->reason6[i].usb,
        t206->reason7[i].lsb,   t206->reason7[i].usb,
        t206->reason8[i].lsb,   t206->reason8[i].usb
        );
 printf ("----\n");
 printf ("\n");


//t207:
 pr_hdr ("type 207 (phasecal and error rate)", t207->record_id, t207->version_no);
 printf ("t207 pcal_mode=%d; ref,rem pcrate=%g, %g\n", t207->pcal_mode, t207->ref_pcrate, t207->rem_pcrate);
 for (int i=0; i<numOfChan; i++)
 {
    printf ("t207 channel[%2d]: Ref: Amp=(l:%8.6f u:%8.6f) Phase=(l:%8.3f u:%8.3f) Offs=(l:%8.3f u:%8.3f) Freq=(l:%9.1f u:%9.1f) errate=%9.6f\n", i,
        t207->ref_pcamp[i].lsb,
        t207->ref_pcamp[i].usb,
        t207->ref_pcphase[i].lsb,
        t207->ref_pcphase[i].usb,
        t207->ref_pcoffset[i].lsb,
        t207->ref_pcoffset[i].usb,
        t207->ref_pcfreq[i].lsb,
        t207->ref_pcfreq[i].usb,
        t207->ref_errate[i]
        );
    printf ("                  Rem: Amp=(l:%8.6f u:%8.6f) Phase=(l:%8.3f u:%8.3f) Offs=(l:%8.3f u:%8.3f) Freq=(l:%9.1f u:%9.1f) errate=%9.6f\n",
        t207->rem_pcamp[i].lsb,
        t207->rem_pcamp[i].usb,
        t207->rem_pcphase[i].lsb,
        t207->rem_pcphase[i].usb,
        t207->rem_pcoffset[i].lsb,
        t207->rem_pcoffset[i].usb,
        t207->rem_pcfreq[i].lsb,
        t207->rem_pcfreq[i].usb,
        t207->rem_errate[i]
        );
 };
 printf ("----\n");
 printf ("\n");


//t208:
 pr_hdr ("type 208 (solution parameter)", t208->record_id, t208->version_no);
 printf ("t208 errcode:'%c' quality:'%c' tapeQcode: '%.6s'\n", 
    t208->errcode,  t208->quality, t208->tape_qcode);
 printf ("t208 a priori: delay= %g rate= %g accel= %g\n",
    t208->adelay, t208->arate, t208->aaccel);
 printf ("t208 SBD: total= %12.6f total@Ref= %12.6f resid= %12.8f std.dev= %g\n",
    t208->tot_sbd,
    t208->tot_sbd_ref,
    t208->resid_sbd,
    t208->sbd_error
    );
 printf ("t208 MBD: total= %12.6f total@Ref= %12.6f resid= %12.8f std.dev= %g\n",
    t208->tot_mbd,
    t208->tot_mbd_ref,
    t208->resid_mbd,
    t208->mbd_error
    );
 printf ("t208 Rat: total= %12.6f total@Ref= %12.6f resid= %12.8f std.dev= %g\n",
    t208->tot_rate,
    t208->tot_rate_ref,
    t208->resid_rate,
    t208->rate_error
    );
 printf ("t208 amplitude= %g totphase= %g totphase@Ref= %g resid.phase= %g\n",
    t208->amplitude,
    t208->totphase,
    t208->totphase_ref,
    t208->resphase
    );
 printf ("t208 inc_seg_ampl= %g inc_chan_ampl= %g\n",
    t208->inc_seg_ampl,
    t208->inc_chan_ampl
    );
 printf ("t208 ambiguity= %g SNR= %g Prob.of false detection= %g\nt208 std dev of tec estimate (TEC units) = %g\n",
    t208->ambiguity,
    t208->snr,
    t208->prob_false,
    t208->tec_error
    );

 printf ("----\n");
 printf ("\n");


//t210:
 pr_hdr ("type 210 (channel data)", t210->record_id, t210->version_no);
 for (int i=0; i<numOfChan; i++)
    printf ("t210 channel[%3d]: amp,phas= (%10.8f, %8.3f)\n", i,
                    t210->amp_phas[i].ampl,
                    t210->amp_phas[i].phase
                    );
 printf ("----\n");
 printf ("\n");


/*
//t211:
 pr_hdr ("type 211 (data slices)", t212->record_id, t212->version_no);
 printf ("----\n");
 printf ("\n");


//t212:
 pr_hdr ("type 212 (AP data)", t212->record_id, t212->version_no);
 printf ("t212 nap = %d\n", t212->nap);
 printf ("t212 first_ap = %d\n", t212->first_ap);
 printf ("t212 channel = %d\n", t212->channel);
 printf ("t212 sbd_chan = %d\n", t212->sbd_chan);

 printf ("t212 data amp = %g\n", t212->data[0].amp);
 printf ("t212 data phase = %g\n", t212->data[0].phase);
 printf ("t212 data weight = %g\n", t212->data[0].weight);

 printf ("----\n");
 printf ("\n");
*/




 clear_mk4fringe(&fringe);
 return 0;
}



#endif

