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
function main()
{
//  const       debug = 'yes';
    const       debug = 'no';
    const       selfName = 'vgosDx.js';
    // check arguments:
    if (args.length < 3)
    {
        print('\nscript ' + selfName + ' usage:\n');
        print(selfName + ' operation input output');
        print('where arguments:');
        print('   operation  -- what to do: either "db2da" or "da2db"');
        print('   input      -- a database name (with or without version part/extension, path) or a wrapper file name');
        print('   output     -- a database name (with or without version part/extension, path) or a wrapper file name');
        logger.returnCode = 1;
        return logger.returnCode;
    };

//  var operation;
    operation = args[0];
    handler.fileName  = args[1];
    output = args[2];

    if (debug === 'yes')
    {
        print('   operation=' + operation);
        print('   input    =' + handler.fileName);
        print('   output   =' + output);
    };

    if (operation == 'b2a')
        operation = 'db2da';
    if (operation == 'a2b')
        operation = 'da2db';

    if (operation == 'db2da')
    {
        handler.inputType = 'VDB';
        if (debug === 'yes')
            print(selfName + ': operation vgosDb => vgosDa');
    }
    else if (operation == 'da2db')
    {
        handler.inputType = 'VDA';
        if (debug === 'yes')
            print(selfName + ': operation vgosDa => vgosDb');
    }
    else
    {
        print(selfName + ': ERROR: unsupported operation "' + operation + '"');
        logger.returnCode = 1;
        return logger.returnCode;
    };

    // check:
    if (handler.inputType == '---')
    {
        print(selfName + ': ERROR: wrong intput type.');
        logger.returnCode = 1;
        return logger.returnCode;
    };
    if (debug === 'yes')
    {
        print(selfName + ': Session name = ' + handler.fileName);
        print(selfName + ': input type   = ' + handler.inputType);
    };

    // set up logging:
    logger.fileName = selfName + '.' + operation + '.' + handler.guessedSessionName + '.log';
    logger.have2store = true;
    // log all error messages:
    logger.addLogFacility(Log.Err, Log.All);
    // log all warning messages:
    logger.addLogFacility(Log.Wrn, Log.All);
    // log all information messages:
    logger.addLogFacility(Log.Inf, Log.All);
    // skip all debug messages:
    logger.delLogFacility(Log.Dbg, Log.All);
    //->logger.addLogFacility(Log.Dbg, Log.All);
    // delete the file if it exists:
    logger.rmLogFile();
    logger.isMute = true;

    logger.write(Log.Inf, Log.Preproc, selfName + ': Starting processing ' + handler.fileName + ' file');
    handler.importSession();

    if (session.isOk)
    {
        if (handler.inputType == 'VDB')
        {
            if (handler.saveDataAsVgosDa(output))
            {
                logger.returnCode = 0;
                if (debug === 'yes')
                    print(selfName + ': Session saved as a vgosDa file "' + output + '"');
            }
            else
            {
                print(selfName + ': Saving session as a vgosDa file "' + output + '" has failed');
                logger.returnCode = 1;
            };
        }
        else if (handler.inputType == 'VDA')
        {
            if (handler.saveDataAsVgosDb(output))
            {
                logger.returnCode = 0;
                if (debug === 'yes')
                    print(selfName + ': Session was saved as a vgosDb file "' + output + '"');
            }
            else
            {
                print(selfName + ': Saving session as a vgosDb file "' + output + '" has failed');
                logger.returnCode = 1;
            };
        };
    }
    else
    {
        print(selfName + ': ERROR: reading the file ' + handler.fileName + ' as ' + handler.inputType + ' type has failed.');
        logger.returnCode = 1;
    };
    return logger.returnCode;
};
// end of main body





main();

