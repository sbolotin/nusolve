//
//    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
//    and is designed to perform data analyis of a geodetic VLBI session.
//    Copyright (C) 2021 Sergei Bolotin.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
//
const   selfName = 'autoINT.js';
var     hasShortBln;
var     need2save;


//
//
function setSigma2add(sigma2add)
{
    var baselines = session.baselines;
    // session-wide:
    for (var i=0; i<baselines.length; i++)
        baselines[i].sigma2add = sigma2add;
    // per band:
    for (var i=0; i<session.bands.length; i++)
    {
        for (var j=0; j<session.bands[i].baselines.length; j++)
            session.bands[i].baselines[j].sigma2add = sigma2add;
    };
};




//
function checkStationNames(phrase)
{
    var count = 0;
    var stations = session.stations;
    for (var i=0; i<stations.length; i++)
    {
        var stn = stations[i];
        if (stn.key.indexOf(phrase) !== -1)
        {
            stn.setIsValid(false);
            logger.write(Log.Inf, Log.Preproc, selfName + ': checkStationNames(): station ' +
                stn.name + ' has been deselected');
            count++;
        };
    };
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkStationNames(): ' + count +
        ' stations have been deselected');
};




//
function checkShortBaselines(limit)
{
    var baselines = session.baselines;
    for (var i=0; i<baselines.length; i++)
    {
        var bln = baselines[i];
        if (bln.length < limit)
        {
            bln.setIsValid(false);
            logger.write(Log.Wrn, Log.Preproc, selfName + ': checkShortBaselines(): the short baseline ' +
                bln.name + ' has been deselected');
            hasShortBln = true;
        };
    };
};



//
function reCheckShortBaselines(limit)
{
    const prevWrms = session.bands[config.activeBandIdx].wrms;

    logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckShortBaselines(): begin');

    var baselines = session.baselines;
    for (var i=0; i<baselines.length; i++)
    {
        var bln = baselines[i];
        if (bln.length < limit && 7 < bln.length)
        {
            if (!bln.isValid)
            {
                logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckShortBaselines(): checking baseline ' + bln.name);
                bln.setIsValid(true);
                session.process();
                if (2*prevWrms < session.bands[config.activeBandIdx].wrms)
                {
                    bln.setIsValid(false);
                    logger.write(Log.Wrn, Log.Preproc, selfName + ': checkShortBaselines(): looks like it is better not to use baseline ' +
                        bln.name + ': the wrms are increasing: ' +  (prevWrms*1.0e12).toFixed(2) + ' => ' +
                        (session.bands[config.activeBandIdx].wrms*1.0e12).toFixed(2) + ' (ps)');
                    session.process();
                }
                else
                {
                    logger.write(Log.Inf, Log.Preproc, selfName + ': checkShortBaselines(): looks like baseline ' +
                        bln.name + ' is ok, wrms: ' +  (prevWrms*1.0e12).toFixed(2) + ' => ' +
                        (session.bands[config.activeBandIdx].wrms*1.0e12).toFixed(2) + ' (ps)');
                };
            }
            else
                logger.write(Log.Wrn, Log.Preproc, selfName + ': checkShortBaselines(): Warning: the baseline ' +
                    bln.name + ' already is turned on');
        };
    };
};



//
// make initial set up of a session:
//
function makeSetup()
{
/*
    var savedUseGoodQualityCodeAtStartup = config.useGoodQualityCodeAtStartup;
    var savedGoodQualityCodeAtStartup    = config.goodQualityCodeAtStartup;
    config.goodQualityCodeAtStartup = 6;
*/
    logger.write(Log.Inf, Log.Preproc, selfName + ': makeSetup(): making initial setup.');
//  config.useGoodQualityCodeAtStartup = false;
    session.zeroIono();
    session.resetAllEditings();
//  config.useGoodQualityCodeAtStartup = savedUseGoodQualityCodeAtStartup;
    logger.write(Log.Inf, Log.Preproc, selfName + ': edit info has been reset');
    //
    // turn off everything that has QC<8:
    session.suppressNotSoGoodObs();

//    config.goodQualityCodeAtStartup    = savedGoodQualityCodeAtStartup;

    //
    // set up a reference clock station:
    session.pickupReferenceClocksStation();
    // set up a reference station station:
    session.pickupReferenceCoordinatesStation();
    // scan correlator's report and search for applied manual phase calibrations:
    session.checkUseOfManualPhaseCals();
    //
    session.setNumOfClockPolynoms4Stations(2);
    //
    setSigma2add(5.0e-9); // 5ns, since we start with SBD
    //
    checkStationNames('BBC');
    //
    hasShortBln = false;
    checkShortBaselines(7); // turn off baselines with length < 7m (usually, tests of BBC on the same antenna)
    //?    session.calcIono(true); // <- only for SB delays
};



//
// resolve ambiguities:
//
function try2resolveAmbiguities()
{
    var maxNumOfPasses  = 0.35*session.numOfObservations;       //
    var threshold       = 4.0;                                  // sigmas
    var bands           = session.bands;
    //
    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Begin of resolving ambiguities.');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Set up estimated parameters (clocks only):');
    logger.write(Log.Inf, Log.Preproc, '');

    parsDescript.unsetAllParameters();
    parsDescript.setMode4Parameter(Parameters.Clocks,   Parameters.EstimateLocal);

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Obtaining a solution for SbDelays:');
    logger.write(Log.Inf, Log.Preproc, '');

    config.useDelayType = CFG.VD_SB_DELAY;      // SBD
    config.activeBandIdx = bands.length - 1;    // S-band
    session.process();

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): the solution obtained.');
    //
    // initially, remove everything that is outp of 7*sigma, no thresholds for absolute values:
    session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.8);
    session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 5.0, -1.0);
//  session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 5.0, 100.0e-9);

    checkShortBaselines(500);
    session.process();

    // if have a short baseline try to turn it back:
    if (hasShortBln)
        reCheckShortBaselines(500);

/*
    if (1 < session.baselines.length)
    {
        config.opMode = CFG.OPM_BAND;
        session.process();
        session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.8);
        session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 5.0, 30.0e-9);
    };
*/

    //
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Switching to GrpDelays:');
    config.useDelayType = CFG.VD_GRP_DELAY;
    config.opMode       = CFG.OPM_BASELINE;
    setSigma2add(20.0e-12); // 20ps, we switched to GrpD

    //
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Checking each band:');
    logger.write(Log.Inf, Log.Preproc, '');
//  for (var i=0; i<bands.length; i++)
    for (var i=bands.length-1; -1<i; i--)
    {
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Processing the ' + bands[i].key + '-band:');
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Scanning ambig.multipliers');
        logger.write(Log.Inf, Log.Preproc, '');

        config.activeBandIdx = i;
        session.scanAmbiguityMultipliers(i);
        session.process();
        session.scanAmbiguityMultipliers(i);
        session.process();
        session.scanAmbiguityMultipliers(i);
        session.process();

        logger.write(Log.Inf, Log.Preproc, '');
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Running another solution');
        logger.write(Log.Inf, Log.Preproc, '');

        session.process();

        logger.write(Log.Inf, Log.Preproc, '');
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Calling eliminateOutliersSimpleMode():');
        logger.write(Log.Inf, Log.Preproc, '');

        // remove 5*sigma or if absolute value are bigger than 0.8 of typical ambig spacing:
        session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.4);
        session.eliminateOutliersSimpleMode(i, maxNumOfPasses, threshold, bands[i].groupDelaysAmbigSpacing*0.3);

        logger.write(Log.Inf, Log.Preproc, '');
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): the band is done.');
        logger.write(Log.Inf, Log.Preproc, '');
    };
    //
    // add parameters and repeat:
    //
    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Setting the number of polynomial terms for station clocks to 3:');
    session.setNumOfClockPolynoms4Stations(3);
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Set up estimated parameters (plus zenith delays and baseline vector):');
    parsDescript.setMode4Parameter(Parameters.Zenith,    Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Bl_Length, Parameters.EstimateLocal);
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Obtaining an intermediate solution:');

    session.process();

    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): a solution is obtained.');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Checking each band again:');
    logger.write(Log.Inf, Log.Preproc, '');

    threshold           = 4.0;          // sigmas
    for (var i=bands.length-1; -1<i; i--)
    {
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Processing the ' + bands[i].key + '-band:');
        logger.write(Log.Inf, Log.Preproc, '');

        config.activeBandIdx = i;
        session.process();
        session.eliminateOutliersSimpleMode(i, maxNumOfPasses, threshold, bands[i].groupDelaysAmbigSpacing*0.25);


        session.scanAmbiguityMultipliers(i);
        session.process();
        session.scanAmbiguityMultipliers(i);
        session.process();
        session.scanAmbiguityMultipliers(i);
        session.process();

        logger.write(Log.Inf, Log.Preproc, '');

        session.process();
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): ' + bands[i].key + '-band: done.');
        logger.write(Log.Inf, Log.Preproc, '');
    };
    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): finished.');
    logger.write(Log.Inf, Log.Preproc, '');
};





//
// check clock breaks:
//
function checkClockBreaks()
{
    var bands = session.bands;
    logger.write(Log.Inf, Log.Preproc, '');

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): Begin of clock break checking.');
    logger.write(Log.Inf, Log.Preproc, '');

    config.activeBandIdx = session.primaryBandIdx;
    config.useDelayType  = CFG.VD_GRP_DELAY;
    logger.write(Log.Inf, Log.Preproc, '');

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): Obtaining an intermediate solution:');
    logger.write(Log.Inf, Log.Preproc, '');

    session.process();

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): done.');
    logger.write(Log.Inf, Log.Preproc, '');

    for (var i=0; i<bands.length; i++)
    {
        logger.write(Log.Inf, Log.Preproc, '');
        logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): Processing the ' + bands[i].key + '-band:');
        logger.write(Log.Inf, Log.Preproc, '');
        session.checkClockBreaks(i);
        logger.write(Log.Inf, Log.Preproc, '');
        logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): done.');
        logger.write(Log.Inf, Log.Preproc, '');
    };
    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): finished.');
    logger.write(Log.Inf, Log.Preproc, '');
};





//
// do reweighting and outliers:
//
function doReweightAndOutliers()
{
    var primaryBand = session.bands[session.primaryBandIdx];
    // make a full solution:
    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): Set up estimated parameters for full solution:');
    logger.write(Log.Inf, Log.Preproc, '');

    session.setNumOfClockPolynoms4Stations(3);
    parsDescript.unsetAllParameters();
    parsDescript.setMode4Parameter(Parameters.Clocks,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Zenith,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.PolusUt1, Parameters.EstimateLocal);
//  parsDescript.setMode4Parameter(Parameters.Bl_Clk,   Parameters.EstimateLocal);

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): done.');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): Configure the solution:');
    logger.write(Log.Inf, Log.Preproc, '');

    config.useDelayType = CFG.VD_GRP_DELAY;
    config.activeBandIdx= session.primaryBandIdx;
    config.opThreshold  = 3.0;
    config.opMode       = CFG.OPM_BASELINE;
    config.opAction     = CFG.OPA_RESTORE;

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): done.');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): Reweighting (first run):');
    logger.write(Log.Inf, Log.Preproc, '');

    session.doReWeighting();

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): done.');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): Begin of reweighting/outlier processing iteration:');
    logger.write(Log.Inf, Log.Preproc, '');

    var numOfRestored = 0;
    var numOfEliminated = 0;
    var roiCounter = 0;
    var mdfData=0;
    do
    {
        if ( (numOfRestored = session.restoreOutliers(session.primaryBandIdx)) )
            session.doReWeighting();
        if ( (numOfEliminated = session.eliminateOutliers(session.primaryBandIdx)) )
            session.doReWeighting();
        mdfData += numOfRestored + numOfEliminated;
        roiCounter++;
    }
    while ((numOfRestored + numOfEliminated) && (7 < primaryBand.numProcessed));

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): End of reweighting/outlier processing iteration. Counter=' +
        roiCounter + ', number of modified observations: ' + mdfData);
    logger.write(Log.Inf, Log.Preproc, '');

    session.process();
    session.process();
    session.process();
    session.process();
    session.process();
    session.process();

    //
    // trun off the flag:
    config.doWeightCorrection = false;
};


//
//
// make some outut:
//
function printInfo()
{
    var sIds;
    sIds = '=';
    for (var i=0; i<session.stations.length; i++)
    {
        sIds += session.stations[i].name;
        if (i < session.stations.length - 1)
            sIds += ':';
    };
    sIds += '=';


    var primaryBand = session.bands[session.primaryBandIdx];
    print(' -- printInfo(): Session name           : ' + session.name);
    print(' -- printInfo(): Session scheduled by   : ' + session.schedulerName);
    print(' -- printInfo(): Session submitted by   : ' + session.submitterName);
    print(' -- printInfo(): Session correlator name: ' + session.correlatorName);
    print(' -- printInfo(): Session official name  : ' + session.officialName);
    print(' -- printInfo(): Session sessionCode    : ' + session.sessionCode);
    print(' -- printInfo(): Session description    : ' + session.description);
    print(' -- printInfo(): Session suffix         : ' + session.networkSuffix);
    print(' -- printInfo(): Session networkID      : ' + session.networkID);
    print(' -- printInfo(): Session #bands         : ' + session.numOfBands);
    print(' -- printInfo(): Session #stations      : ' + session.numOfStations);
    print(' -- printInfo(): Session #baseline      : ' + session.numOfBaselines);
    print(' -- printInfo(): Session #sources       : ' + session.numOfSources);
    print(' -- printInfo(): Session #observs       : ' + session.numOfObservations);
    print(' -- printInfo(): Session created on     : ' + session.tCreation);
    print(' -- printInfo(): Session started on     : ' + session.tStart);
    print(' -- printInfo(): Session stopped on     : ' + session.tFinis);
    print(' -- printInfo(): Session mean epoch     : ' + session.tMean);

    print(' -- printInfo(): Primary band key       : ' + primaryBand.key);
    print(' -- printInfo(): Epoch of the first obs : ' + primaryBand.tFirst);
    print(' -- printInfo(): Epoch of the last obs  : ' + primaryBand.tLast);
    print(' -- printInfo(): Number of total obs    : ' + primaryBand.numTotal);
    print(' -- printInfo(): Number of usable obs   : ' + primaryBand.numUsable);
    print(' -- printInfo(): Number of used obs     : ' + primaryBand.numProcessed);
    print(' -- printInfo(): WRMS                   : ' + (primaryBand.wrms*1.0e12).toFixed(2) + ' (ps)');
    print(' -- printInfo(): DoF                    : ' + primaryBand.dof);
    print(' -- printInfo(): Chi^2/DoF              : ' + (primaryBand.chi2/primaryBand.dof).toFixed(4));

    print(' -- printInfo(): Numbers                : ' + primaryBand.numTotal + '/' + primaryBand.numUsable + '/' + primaryBand.numProcessed +
        '  ' + (primaryBand.wrms*1.0e12).toFixed(2) + '    UT1= ' + (session.dUt1Value*86400*1.0e6).toFixed(2) +
        '  ' + (session.dUt1Correction*86400*1.0e6).toFixed(2) + '  ' + (session.dUt1StdDev*86400*1.0e6).toFixed(2) +
        '  ' + sIds);

};




function main()
{
    const       pwd = setup.pwd;
    const       sfx = "/o-aut";

    need2save = false;

    // check arguments:
    if (!args.length)
    {
        print('\nscript ' + selfName + ' usage:\n');
        print(selfName + ' input');
        print('where arguments:');
        print('   input      -- a database name (with or without version part) or a wrapper file name');
        return;
    };

    if (1 < args.length)
    {
        if (args[1] == 'yes')
            need2save = true;
    };


    handler.fileName  = args[0];
    handler.inputType = 'VDB';
    //
    //
    //
    setup.path2VgosDbFiles     = pwd + '/vgosDb';
    setup.path2SpoolFileOutput = pwd + sfx + '/Spools';
    setup.path2ReportOutput    = pwd + sfx + '/Reports';
//  setup.have2KeepSpoolFileReports = true;

    // set up logging:
    logger.dirName = pwd + sfx + '/Logs';
    logger.fileName = handler.guessedSessionName + '.log';
    logger.have2store = true;
    logger.rmLogFile();
    logger.isMute = true;
    logger.write(Log.Inf, Log.Preproc, selfName + ': Starting processing ' + handler.fileName + ' file');


    handler.importSession();
    //
    //
    if (session.isOk)
    {
        //
        makeSetup();
        //
        try2resolveAmbiguities();
        //
//        checkClockBreaks();
        //
        config.activeBandIdx = session.primaryBandIdx;
        logger.write(Log.Inf, Log.Preproc, selfName + ': calculation inosphere corrections:');
        session.calcIono();
        logger.write(Log.Inf, Log.Preproc, selfName + ': calculation inosphere corrections done');
        logger.write(Log.Inf, Log.Preproc, selfName + ': obtaining another solution');
        session.process();
        logger.write(Log.Inf, Log.Preproc, selfName + ': obtaining another solution: done');
        //
        doReweightAndOutliers();
        //
        if (need2save)
           handler.saveResults();
        //
        handler.generateReport();
        //
        printInfo();
        //
        logger.write(Log.Inf, Log.Preproc, selfName + ': the session is done: ' + handler.fileName);
    }
    else
        logger.write(Log.Err, Log.Preproc, selfName + ': session reading failed: ' + handler.fileName);
};










main();




//for(var key in config) print(key + ' => ' + config[key]);







