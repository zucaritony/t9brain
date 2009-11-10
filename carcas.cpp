if(len==0) {
					cout << "first letter of the word\n";
					
					//if they need more letters then they'll push more
					//if not, then they'll have at least 3 to cycle through
					string lower;
					for(int i=0; i<nletts; i++) {
						//only way to get C++ to actually add a character as a string...
						string dummy = "";
						dummy += letters[nump][i];
				
						possible.insert(dummy);
					//	curr_dict.insert(dummy);
					}
					
					len++;
					
					//open up the dictionary file for the specific word list, based on what was pressed
					string wordloc = "words/";
					wordloc += letters[nump];
					dict.open(wordloc.c_str());
					while(dict >> dword)
						curr_dict.insert(dword);
					dict.close();
			//for the sake of speed, I'm ignoring the first-letter searching routine
			/*
					//speedup, plus the first while stanza, reduce the amount of comparisons being made
					//by skipping to the part of the dictionary whose words carry the correct first letter
					//and by skipping the letters that are no longer relevant, seeing as the dictionary is alphabetical
					unsigned short speedup = 0;
			
					//skip to the beginning of the relevant section of the dictionary
					//keep in mind that if I swapped the parts before and after the "&&", this would skip the first relevant word
					while(dict >> dword && tolower(dword[0]) != letters[nump][0]);
			
					//now, starting with the last word we just grabbed (the first relevant one)
					//we are going to store all of the words that have the correct first letter
					//and start skipping the letters we look for once we find the first one that doesn't work
			
					//this is set to do the full loop through the letters twice because of the ASCIIbetical nature of the dictionary
					bool capitals = true;
					do {			
						//speedup just makes it skip letters that are no longer relevant by starting i at a higher reference value
						for(int i=0+speedup; i<nletts; i++) {
						//	cout << "and here --- \'" << letters[nump][i] << "\'\n";
							if(tolower(dword[0]) == letters[nump][i]) {
								curr_dict.insert(dword);
								break;
							} else
								speedup++;
						}
				
						//here we reset it (should only happen once) once we get through the
						//capital letter words to deal with the lower case by fast forwarding
						if(speedup==nletts-1 && capitals) {
							speedup=0;
							capitals = false;
					
							//here is the fast forward...get to the beginning of the lower case letters
							while(dict >> dword && tolower(dword[0]) != letters[nump][0]);
						}
				
					} while(dict >> dword && speedup<nletts);
			
		//			cout << curr_dict.size() << "\n";
		
			*/
				} else if(len>=1) {
		//			cout << "more than 1 letter\n";
			
					vector<bool> dict_list, poss_list;
					bool dict_use;
					set<string>::iterator dict_iter, poss_iter;
					int i, j;
			
					//make every possible combination of word prefixes, whether or not they are useful
					//and store them in the original 'possible' set
					for(curr = possible.begin(); curr != possible.end(); curr++) {
						poss_list.push_back(false);
						for(i=0; i<nletts; i++) {
							poss_temp.insert((*curr)+letters[nump][i]);
						}
					}
					possible.swap(poss_temp);
					poss_temp.clear();
					len++;
			
		//			cout << curr_dict.size() << "   updated possible\n";
			
					//i and j are the current locations in the 'possible' and 'curr_dict' sets for reference with their
					//removal-marking vector counterparts, 'poss_list' and 'dict_list' respectively
					for(i=0, dict_iter = curr_dict.begin(); dict_iter != curr_dict.end(); dict_iter++, i++) {
		//				cout << "herehere\n";
						dict_list.push_back(false);
						for(j=0, poss_iter = possible.begin(); poss_iter != possible.end(); poss_iter++, j++) {
							//if we find a use for the possible word prefix and the dictionary word, we keep track of it
							if(frocmp((*poss_iter), (*dict_iter))) {
		//						cout << poss_list.size() << "  " << dict_list.size() << "  found match\n";
								poss_list[j] = dict_list[i] = true;
							}
						}
					}
			
		//			cout << "after that\n";
					for(i=0, curr = possible.begin(); curr != possible.end(); curr++, i++) {
						if(poss_list[i])
							poss_temp.insert((*curr));
					}
			
					//if we didn't find ANYTHING then we just ignore the last press
					if(poss_temp.size()>0) {
						possible.clear();
						possible.swap(poss_temp);
//						poss_temp.clear();
					} else break;

					for(i=0, curr = curr_dict.begin(); curr != curr_dict.end(); curr++, i++) {
						if(dict_list[i])
							dict_temp.insert((*curr));
					}
					curr_dict.clear();
					curr_dict.swap(dict_temp);
//					dict_temp.clear();
				}
