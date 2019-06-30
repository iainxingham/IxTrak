/*  Instructions to create the tables for the IxTrak database
 *
 *  Produce the database with
 *
 *		cat db.sql | sqlite3 ixtrak.sqlite
 */

CREATE TABLE pats (id INTEGER PRIMARY KEY, 
                   rxr TEXT, 
                   nhs TEXT);

CREATE TABLE physiology (id INTEGER PRIMARY KEY,
                         pat_id INTEGER,
                         phys_date TEXT,
                         phys_type INTEGER,
                         phys_value REAL,
                         ref_high REAL,
                         ref_low REAL,
                         details TEXT);
                         
CREATE TABLE dvla (inv_number INTEGER,
                   pat_id INTEGER,
                   request_date TEXT,
                   report_date TEXT);                
                         
CREATE TABLE device (id INTEGER PRIMARY KEY,
                     manufacturer TEXT,
                     model TEXT,
                     dev_type TEXT,
                     serial_no TEXT,
                     service_date TEXT);
                     
CREATE TABLE deployed_dev (pat_id INTEGER,
                           dev_id INTEGER,
                           issue_date TEXT,
                           withdraw_date TEXT,
                           dev_mode TEXT);
                         
CREATE TABLE phys_types (id INTEGER PRIMARY KEY,
                         measure TEXT,
                         ref_high REAL,
                         ref_low REAL);
                         
CREATE TABLE interaction (id INTEGER PRIMARY KEY,
						  interact_date TEXT,
						  pat_id INTEGER,
						  interact_type INTEGER,
						  disposal INTEGER,
						  for_radiology INTEGER,
						  admit INTEGER,
						  seen_by INTEGER,
						  diagnosis TEXT,
						  follow_up INTEGER,
						  notes TEXT);
						  
CREATE TABLE clinician (id INTEGER PRIMARY KEY,
                        name TEXT,
                        grade INTEGER);
                        
CREATE TABLE investigation (pat_id INTEGER,
                            interact_id INTEGER,
                            inv_type INTEGER,
                            request_date TEXT,
                            close_date TEXT,
                            notes TEXT);
                            
CREATE TABLE referral (pat_id INTEGER,
                       interact_id INTEGER,
                       request_date TEXT,
                       speciality TEXT);
                       
CREATE TABLE radiology (pat_id INTEGER,
                        interact_id INTEGER,
                        request_date TEXT,
                        close_date TEXT);
                        
CREATE TABLE clin_grades (id INTEGER PRIMARY KEY,
                          name TEXT);
                          
CREATE TABLE inv_types (id INTEGER PRIMARY KEY,
                        name TEXT);
                        
CREATE TABLE contact_types (id INTEGER PRIMARY KEY,
                            name TEXT);
                            
CREATE TABLE disp_types (id INTEGER PRIMARY KEY,
                         name TEXT);
                                               
					  
                           
INSERT INTO "phys_types"("measure", "ref_high", "ref_low") VALUES ("FeNO", "25", "5");
INSERT INTO "phys_types"("measure") VALUES ("Height");
INSERT INTO "phys_types"("measure") VALUES ("Weight");

INSERT INTO "clin_grades"("name") VALUES ("Consultant");
INSERT INTO "clin_grades"("name") VALUES ("ST3+");
INSERT INTO "clin_grades"("name") VALUES ("FY or CT");
INSERT INTO "clin_grades"("name") VALUES ("Nurse practitioner");

INSERT INTO "inv_types"("name") VALUES ("Sleep study (legacy)");
INSERT INTO "inv_types"("name") VALUES ("Pulmonary function tests");
INSERT INTO "inv_types"("name") VALUES ("6 minute walk");
INSERT INTO "inv_types"("name") VALUES ("Overnight oximetry");
INSERT INTO "inv_types"("name") VALUES ("Limited polysomnography");
INSERT INTO "inv_types"("name") VALUES ("Oasys diary");
INSERT INTO "inv_types"("name") VALUES ("Peak flow diary");
INSERT INTO "inv_types"("name") VALUES ("CT thorax");
INSERT INTO "inv_types"("name") VALUES ("IgE");
INSERT INTO "inv_types"("name") VALUES ("Functional antibodies");
INSERT INTO "inv_types"("name") VALUES ("Aspergillus IgG");
INSERT INTO "inv_types"("name") VALUES ("Immunology");
INSERT INTO "inv_types"("name") VALUES ("Neurophysiology");
INSERT INTO "inv_types"("name") VALUES ("Bronchoscopy");
INSERT INTO "inv_types"("name") VALUES ("Echocardiogram");
INSERT INTO "inv_types"("name") VALUES ("Exercise tolerance test");
INSERT INTO "inv_types"("name") VALUES ("Other");
INSERT INTO "inv_types"("name") VALUES ("MIP / MEP");

INSERT INTO "contact_types"("name") VALUES ("Clinic - new");
INSERT INTO "contact_types"("name") VALUES ("Clinic - follow up");
INSERT INTO "contact_types"("name") VALUES ("Critical care");
INSERT INTO "contact_types"("name") VALUES ("Advice (legacy)");
INSERT INTO "contact_types"("name") VALUES ("Ward");
INSERT INTO "contact_types"("name") VALUES ("RAU");
INSERT INTO "contact_types"("name") VALUES ("Results");
INSERT INTO "contact_types"("name") VALUES ("Pre clinic investigation");
INSERT INTO "contact_types"("name") VALUES ("Radiology meeting");
INSERT INTO "contact_types"("name") VALUES ("Ad hoc");
INSERT INTO "contact_types"("name") VALUES ("Administration");
INSERT INTO "contact_types"("name") VALUES ("Advice and guidance");
INSERT INTO "contact_types"("name") VALUES ("Telephone consultation");

INSERT INTO "disp_types"("name") VALUES ("Discharge");
INSERT INTO "disp_types"("name") VALUES ("Follow up");
INSERT INTO "disp_types"("name") VALUES ("DNA - discharge");
INSERT INTO "disp_types"("name") VALUES ("DNA - further appointment");
INSERT INTO "disp_types"("name") VALUES ("RAU appointment");
INSERT INTO "disp_types"("name") VALUES ("Other / pending");
INSERT INTO "disp_types"("name") VALUES ("Write with results");

