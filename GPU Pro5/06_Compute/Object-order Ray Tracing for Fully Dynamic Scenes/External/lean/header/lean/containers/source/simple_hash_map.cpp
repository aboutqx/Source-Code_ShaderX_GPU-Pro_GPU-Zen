#ifdef LEAN_BUILD_LIB
#include "../../depconfig.h"
#endif

#include "../simple_hash_map.h"
#include <algorithm>

// Gets the first prime number available that is greater than or equal to the given capacity,
// may only return a prime number smaller than the given capacity when the actual result would
// be greater than the given maximum value.
LEAN_MAYBE_LINK size_t lean::containers::impl::next_prime_capacity(size_t capacity, size_t max)
{
	// Enforce capacity <= max
	capacity = min(capacity, max);

	// Prime numbers growing by 8%
	static const uint4 primes[] = {
		2U, 3U, 5U, 7U, 11U, 13U, 17U, 19U, 23U, 29U, 31U,
		37U, 41U, 43U, 47U, 53U, 59U, 61U, 67U, 71U, 73U, 79U,     
		83U, 89U, 97U, 103U, 109U, 113U, 127U, 137U, 139U, 149U,     
		157U, 167U, 179U, 193U, 199U, 211U, 227U, 241U, 257U,     
		277U, 293U, 313U, 337U, 359U, 383U, 409U, 439U, 467U,   
		503U, 541U, 577U, 619U, 661U, 709U, 761U, 823U, 887U,    
		953U, 1031U, 1109U, 1193U, 1289U, 1381U, 1493U, 1613U,     
		1741U, 1879U, 2029U, 2179U, 2357U, 2549U, 2753U, 2971U,    
		3209U, 3469U, 3739U, 4027U, 4349U, 4703U, 5087U, 5503U,    
		5953U, 6427U, 6949U, 7517U, 8123U, 8783U, 9497U, 10273U,    
		11113U, 12011U, 12983U, 14033U, 15173U, 16411U, 17749U,    
		19183U, 20753U, 22447U, 24281U, 26267U, 28411U, 30727U,    
		33223U, 35933U, 38873U, 42043U, 45481U, 49201U, 53201U,      
		57557U, 62233U, 67307U, 72817U, 78779U, 85229U, 92203U,     
		99733U, 107897U, 116731U, 126271U, 136607U, 147793U,     
		159871U, 172933U, 187091U, 202409U, 218971U, 236897U,    
		256279U, 277261U, 299951U, 324503U, 351061U, 379787U,    
		410857U, 444487U, 480881U, 520241U, 562841U, 608903U,     
		658753U, 712697U, 771049U, 834181U, 902483U, 976369U,     
		1056323U, 1142821U, 1236397U, 1337629U, 1447153U, 1565659U, 
		1693859U, 1832561U, 1982627U, 2144977U, 2320627U, 2510653U,    
		2716249U, 2938679U, 3179303U, 3439651U, 3721303U, 4026031U,     
		4355707U, 4712381U, 5098259U, 5515729U, 5967347U, 6456007U,     
		6984629U, 7556579U, 8175383U, 8844859U, 9569143U, 10352717U,     
		11200489U, 12117689U, 13109983U, 14183539U, 15345007U,       
		16601593U, 17961079U, 19431899U, 21023161U, 22744717U,     
		24607243U, 26622317U, 28802401U, 31160981U, 33712729U,    
		36473443U, 39460231U, 42691603U, 46187573U, 49969847U,     
		54061849U, 58488943U, 63278561U, 68460391U, 74066549U,    
		80131819U, 86693767U, 93793069U, 101473717U, 109783337U,   
		118773397U, 128499677U, 139022417U, 150406843U, 162723577U,  
		176048909U, 190465427U, 206062531U, 222936881U, 241193053U,    
		260944219U, 282312799U, 305431229U, 330442829U, 357502601U,    
		386778277U, 418451333U, 452718089U, 489790921U, 529899637U,    
		573292817U, 620239453U, 671030513U, 725980837U, 785430967U,     
		849749479U, 919334987U, 994618837U, 1076067617U, 1164186217U,   
		1259520799U, 1362662261U, 1474249943U, 1594975441U,       
		1725587117U, 1866894511U, 2019773507U, 2185171673U,     
		2364114217U, 2557710269U, 2767159799U, 2993761039U,      
		3238918481U, 3504151727U, 3791104843U, 4101556399U,
		4294967291U,

		// Sentinel
		4294967291U
	};
	// Exclude sentinel, thus value returned by lower_bound is always valid.
	static const size_t primeCount = sizeof(primes) / sizeof(primes[0]) - 1U;

	// 32 bit size or request <= primeThreshold
	if (sizeof(size_t) < sizeof(uint8) || capacity <= 4294967291U)
	{
		const uint4 *primeCapacity = std::lower_bound(primes, primes + primeCount, capacity);

		LEAN_ASSERT(max >= 2U);

		// *primeCapacity is at worst the first prime number greater than max
		if (*primeCapacity > max)
		{
			--primeCapacity;
			LEAN_ASSERT(primeCapacity >= primes);
			LEAN_ASSERT(*primeCapacity < max);
		}

		// Legal as max is of type size_t and *primeCapacity is less-or-equal than max
		return static_cast<size_t>(*primeCapacity);
	}
	else
	{
		// More prime numbers growing by 8%
		static const uint8 largePrimes[] = {
			// Sentinel
			4294967291U,

			4429680911ULL, 4784055401ULL, 5166779851ULL, 5580122297ULL, 6026532101ULL,
			6508654673ULL, 7029347053ULL, 7591694819ULL, 8199030421ULL, 8854952867ULL,
			9563349119ULL, 10328417069ULL, 11154690451ULL, 12047065699ULL, 13010830967ULL,
			14051697461ULL, 15175833269ULL, 16389899941ULL, 17701091957ULL, 19117179373ULL,
			20646553723ULL, 22298278033ULL, 24082140281ULL, 26008711511ULL, 28089408449ULL,
			30336561173ULL, 32763486083ULL, 35384564987ULL, 38215330193ULL, 41272556651ULL,
			44574361187ULL, 48140310083ULL, 51991534919ULL, 56150857753ULL, 60642926381ULL,
			65494360541ULL, 70733909347ULL, 76392622079ULL, 82504031879ULL, 89104354399ULL,
			96232702793ULL, 103931318963ULL, 112245824501ULL, 121225490437ULL, 130923529673ULL,
			141397412111ULL, 152709205057ULL, 164925941419ULL, 178120016741ULL, 192369618101ULL,
			207759187511ULL, 224379922513ULL, 242330316311ULL, 261716741621ULL, 282654080953ULL,
			305266407451ULL, 329687720017ULL, 356062737617ULL, 384547756627ULL, 415311577169ULL,
			448536503321ULL, 484419423629ULL, 523172977573ULL, 565026815681ULL, 610228960949ULL,
			659047277803ULL, 711771060031ULL, 768712744871ULL, 830209764437ULL, 896626545649ULL,
			968356669213ULL, 1045825202761ULL, 1129491218971ULL, 1219850516501ULL, 1317438557807ULL,
			1422833642437ULL, 1536660333827ULL, 1659593160593ULL, 1792360613377ULL, 1935749462467ULL,
			2090609419501ULL, 2257858173013ULL, 2438486826851ULL, 2633565772987ULL, 2844251034907ULL,
			3071791117649ULL, 3317534407043ULL, 3582937159577ULL, 3869572132343ULL, 4179137902963ULL,
			4513468935179ULL, 4874546449987ULL, 5264510165987ULL, 5685670979293ULL, 6140524657601ULL,
			6631766630263ULL, 7162307960627ULL, 7735292597519ULL, 8354116005307ULL, 9022445285701ULL,
			9744240908581ULL, 10523780181223ULL, 11365682595727ULL, 12274937203381ULL, 13256932179649ULL,
			14317486754059ULL, 15462885694489ULL, 16699916549977ULL, 18035909873899ULL, 19478782663799ULL,
			21037085276951ULL, 22720052099071ULL, 24537656266997ULL, 26500668768337ULL, 28620722269831ULL,
			30910380051433ULL, 33383210455559ULL, 36053867292013ULL, 38938176675341ULL, 42053230809371ULL,
			45417489274139ULL, 49050888416039ULL, 52974959489371ULL, 57212956248491ULL, 61789992748369ULL,
			66733192168217ULL, 72071847541697ULL, 77837595345043ULL, 84064602972611ULL, 90789771210419ULL,
			98052952907333ULL, 105897189139817ULL, 114368964271087ULL, 123518481412687ULL, 133399959925747ULL,
			144071956719767ULL, 155597713257341ULL, 168045530317919ULL, 181489172743357ULL, 196008306562823ULL,
			211688971087873ULL, 228624088774889ULL, 246914015876867ULL, 266667137147029ULL, 288000508118773ULL,
			311040548768281ULL, 335923792669783ULL, 362797696083323ULL, 391821511770037ULL, 423167232711623ULL,
			457020611328517ULL, 493582260234787ULL, 533068841053571ULL, 575714348337871ULL, 621771496204883ULL,
			671513215901351ULL, 725234273173393ULL, 783253015027279ULL, 845913256229507ULL, 913586316727801ULL,
			986673222066011ULL, 1065607079831353ULL, 1150855646217793ULL, 1242924097915217ULL, 1342358025748463ULL,
			1449746667808337ULL, 1565726401232969ULL, 1690984513331611ULL, 1826263274398153ULL, 1972364336349989ULL,
			2130153483258011ULL, 2300565761918687ULL, 2484611022872117ULL, 2683379904701879ULL, 2898050297078077ULL,
			3129894320844281ULL, 3380285866511837ULL, 3650708735832773ULL, 3942765434699387ULL, 4258186669475327ULL,
			4598841603033361ULL, 4966748931276059ULL, 5364088845778147ULL, 5793215953440343ULL, 6256673229715583ULL,
			6757207088092873ULL, 7297783655140267ULL, 7881606347551487ULL, 8512134855355583ULL, 9193105643784143ULL,
			9928554095286847ULL, 10722838422909689ULL, 11580665496742483ULL, 12507118736481961ULL, 13507688235400421ULL,
			14588303294232511ULL, 15755367557771069ULL, 17015796962392831ULL, 18377060719384183ULL, 19847225576934919ULL,
			21435003623089711ULL, 23149803912937001ULL, 25001788225971847ULL, 27001931284049591ULL, 29162085786773521ULL,
			31495052649715469ULL, 34014656861692667ULL, 36735829410628069ULL, 39674695763478383ULL, 42848671424556587ULL,
			46276565138521177ULL, 49978690349602787ULL, 53976985577571037ULL, 58295144423776721ULL, 62958755977678849ULL,
			67995456455893139ULL, 73435092972364709ULL, 79309900410153757ULL, 85654692442966039ULL, 92507067838403357ULL,
			99907633265475613ULL, 107900243926713703ULL, 116532263440850723ULL, 125854844516118791ULL, 135923232077408311ULL,
			146797090643601029ULL, 158540857895089139ULL, 171224126526696283ULL, 184922056648832071ULL, 199715821180738573ULL,
			215693086875197671ULL, 232948533825213499ULL, 251584416531230621ULL, 271711169853729031ULL, 293448063442027349ULL,
			316923908517389531ULL, 342277821198780751ULL, 369660046894683203ULL, 399232850646257863ULL, 431171478697958569ULL,
			465665196993795233ULL, 502918412753298863ULL, 543151885773562841ULL, 586604036635447829ULL, 633532359566283689ULL,
			684214948331586437ULL, 738952144198113299ULL, 798068315733962263ULL, 861913780992679397ULL, 930866883472093723ULL,
			1005336234149861521ULL, 1085763132881850509ULL, 1172624183512398619ULL, 1266434118193390651ULL, 1367748847648861771ULL,
			1477168755460770829ULL, 1595342255897632519ULL, 1722969636369443081ULL, 1860807207278998021ULL, 2009671783861317889ULL,
			2170445526570223663ULL, 2344081168695841793ULL, 2531607662191509001ULL, 2734136275166829739ULL, 2952867177180176387ULL,
			3189096551354590729ULL, 3444224275462959617ULL, 3719762217499996699ULL, 4017343194899996179ULL, 4338730650491996219ULL,
			4685829102531356677ULL, 5060695430733864967ULL, 5465551065192574999ULL, 5902795150407981197ULL, 6375018762440619061ULL,
			6885020263435869323ULL, 7435821884510738489ULL, 8030687635271598091ULL, 8673142646093326381ULL, 9366994057780793437ULL,
			10116353582403256369ULL, 10925661868995516457ULL, 11799714818515161101ULL, 12743692003996373039ULL, 13763187364316084263ULL,
			14864242353461370929ULL, 16053381741738279023ULL, 17337652281077342261ULL, 18446744073709551557ULL,
			
			// Sentinel
			18446744073709551557ULL
		};
		// Exclude end sentinel, thus value returned by lower_bound is always valid.
		static const size_t largePrimeCount = sizeof(largePrimes) / sizeof(largePrimes[0]) - 1U;

		const uint8 *primeCapacity = std::lower_bound(largePrimes, largePrimes + largePrimeCount, capacity);

		LEAN_ASSERT(max > 4294967291U);

		// *primeCapacity is at worst the first prime number greater than max
		if (*primeCapacity > max)
		{
			--primeCapacity;
			LEAN_ASSERT(primeCapacity >= largePrimes);
			LEAN_ASSERT(*primeCapacity < max);
		}

		// Legal as max is of type size_t and *primeCapacity is less-or-equal than max
		return static_cast<size_t>(*primeCapacity);
	}
}
