#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------
# Automate generation of unit test documentation from testcase description file

import sys
import os
import logging.config
import argparse
import yaml
import UTCriteria
import docx
from docx.shared import Cm
import re
from datetime import datetime


def generate_doc_header(document, revision, date):
    section = document.sections[0]
    header = section.header
    paragraph = header.paragraphs[0]
    paragraph.text = "BL EP Software\t\tKW45 HADM HAL Unitary Testing\t\tRev {}, {}".format(revision, date)
    #paragraph.style = document.styles["Header"]


def generate_doc_testcase(document, id, name, descr, params, criteria):
    document.add_heading(name, level=2)

    table = document.add_table(rows=4, cols=2, style='Table Grid')
    table.allow_autofit = False
    table.autofit = False
    for row in table.rows:
        row.cells[0].width = Cm(3)
        row.cells[1].width = Cm(14)

    row = 0
    table.rows[row].cells[0].text = 'Id'
    table.rows[row].cells[1].text = id
    row += 1
    table.rows[row].cells[0].text = 'Objective'
    table.rows[row].cells[1].text = descr
    row += 1
    table.rows[row].cells[0].text = 'Arguments'
    table.rows[row].cells[1].text = params
    row += 1
    table.rows[row].cells[0].text = 'Criteria'
    table.rows[row].cells[1].text = '\n'.join(criteria)
    row += 1

    #p = document.add_paragraph('')

def descr_from_name(name):
    descr = name
    # If name contains T_FCS and T_IP, extract them
    match = re.search('(.*) ([0-9]+)/([0-9]+)/([0-9]+)', name)
    if match:
        descr = match.group(1)
        descr += ' T_FCS=' + match.group(2)
        descr += ' T_IP=' + match.group(3)
        descr += ' T_PM=' + match.group(4)
    descr = 'Validates configuration "' + descr + '"'
    return descr

def main(argv):
    log = logging.getLogger(__name__)
    log.setLevel(logging.DEBUG)
    log.info('Working on %s' % sys.platform)

    parser = argparse.ArgumentParser(description='Automate generation of unit test documentatin from testcase description file.')
    parser.add_argument('--output', action='store', type=str, help='folder name within "records" directory (timestamp used otherwise)')
    parser.add_argument('--testfile', action='store', type=str, default='range_ut_hal.yml', help='testcase input file')
    parser.add_argument('--target', action='store', type=str, default='', help='specify an output target (xlsx, cxlsx; default: None)')

    args = parser.parse_args()

    # Load and check input test file
    try:
        with open(args.testfile, "r") as stream:
            try:
                tests = yaml.load(stream, Loader=yaml.FullLoader)
                assert isinstance(tests, dict), 'YAML file error: must be a dict'
            except yaml.YAMLError as e:
                print(e)
                exit(1)
    except IOError as e:
        print(e)
        exit(1)

    # Output folder will be . by default or ./<output> or absolute path if specified
    if args.output is not None:
        if os.path.isabs(args.output):
            # Absolute path is provided
            filepath = args.output
        else:
            # Relative path is provided: relates to records folder
            filepath = './' + args.output
    else:
        filepath = './'

    # Build .docx output file name
    outfilename = filepath + '/' + os.path.splitext(os.path.basename(args.testfile))[0] + '.docx'

    # Create document from template
    document = docx.Document("hadm_ut_spec_template.docx")
    generate_doc_header(document, '1.0', datetime.today().strftime('%Y-%m-%d'))

    # Collect HADM acceptance criteria
    hadmCrieria = UTCriteria.HadmResultVerdict()

    # Iterate on testcase description file
    for test in tests['testcases']:
        if 'descr' in test and test['descr'] != '':
            descr = test['descr']
        else:
            descr = descr_from_name(test['name'])
        criteria_list = hadmCrieria.get_criteria_per_category(test['cat'] if 'cat' in test else [])
        id = "test_hadm_hal_{:03d}".format(test['id'])
        generate_doc_testcase(document, id, test['name'], descr, test['params'], criteria_list)

    document.save(outfilename)
    print("Document generated: " + outfilename)

if __name__ == '__main__':
    main(sys.argv)
