//
function main()
{
    const       selfName = 'export2ngs.js';
    // check arguments:
    if (!args.length)
    {
        print('\nscript ' + selfName + ' usage:\n');
        print(selfName + ' input [input type]');
        print('where arguments:');
        print('   input      -- a database name (with or without version part) or a wrapper file name');
        print('   input type -- either \'VDB\' (default), \'DBH\' or \'VDA\', optional.');
        return;
    };

    handler.fileName  = args[0];
    handler.inputType = 'VDB';
    if (args.length > 1)
        handler.inputType = args[1];
    // check:
    if (handler.inputType == '---')
    {
        print(selfName + ': ERROR: wrong intput type.');
        return;
    };
    print(selfName + ': Session name = ' + handler.fileName);
    print(selfName + ': input type   = ' + handler.inputType);

    // set up logging:
    logger.fileName = selfName + '.' + handler.fileName + '.log';
    logger.have2store = true;
    // log all error messages:
    logger.addLogFacility(Log.Err, Log.All);
    // log all warning messages:
    logger.addLogFacility(Log.Wrn, Log.All);
    // log all information messages:
    logger.addLogFacility(Log.Inf, Log.All);
    // skip all debug messages:
    logger.delLogFacility(Log.Dbg, Log.All);
    // delete the file if it exists:
    logger.rmLogFile();
    logger.isMute = true;

    logger.write(Log.Inf, Log.Preproc, selfName + ': Starting processing ' + handler.fileName + ' file');

    handler.importSession();
    if (session.isOk)
    {
        print(selfName + ': Session loaded.');
        handler.exportDataToNgs();
        print(selfName + ': Session exported as an NGS file.');
    }
    else
        print(selfName + ': ERROR: reading the file ' + handler.fileName + ' as ' + handler.inputType + ' type has failed.');
};
// end of main body


main();

