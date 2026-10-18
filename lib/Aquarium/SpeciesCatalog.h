#ifndef SPECIES_CATALOG_H
#define SPECIES_CATALOG_H

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

enum class SilhouetteKind : uint8_t {
  Schooler = 0,
  ChromisShoal,
  Fusilier,
  Anthias,
  Cardinal,
  Sergeant,
  Sweeper,
  Silverside,
  Dartfish,
  Surgeon,
  Unicorn,
  Clownfish,
  BlueTang,
  YellowTang,
  MoorishIdol,
  Turtle,
  Star,
  Snake,
  Octopus,
  Shark,
  WhaleShark,
  Manta,
  Whale,
  Prop
};

enum class SizeClass : uint8_t { Tiny = 0, Small, Medium, Large };

enum class SpeciesId : uint8_t {
  BlueGreenChromis = 0,
  BlackAxilChromis,
  BlackbarChromis,
  BicolorChromis,
  AmbonChromis,
  TernateChromis,
  WebersChromis,
  BarrierReefChromis,
  DarkfinChromis,
  DeepReefChromis,
  YellowChromis,
  SpinyChromis,
  HalfAndHalfChromis,
  MidgetChromis,
  OvateChromis,
  PaleChromis,
  PearlChromis,
  SapphireChromis,
  ScissortailChromis,
  StaghornChromis,
  SurgeChromis,
  TwinspotChromis,
  VanderbiltsChromis,
  WhitetailChromis,
  FusilierDamsel,
  GreenChromis,
  IndigoChromis,
  ReefChromis,
  BlueAndYellowFusilier,
  BluestreakFusilier,
  RuddyFusilier,
  GoldbandFusilier,
  DoubleLinedFusilier,
  YellowbackFusilier,
  LunarFusilier,
  MojicaFusilier,
  StriatedFusilier,
  WideBandFusilier,
  LyretailAnthias,
  ThreadfinAnthias,
  PurpleAnthias,
  BartlettsAnthias,
  SquarebackAnthias,
  FlameAnthias,
  ResplendentAnthias,
  RedfinAnthias,
  YellowbackAnthias,
  PrincessAnthias,
  SunshineAnthias,
  DisparAnthias,
  EvansiAnthias,
  LoriAnthias,
  PasculAnthias,
  RandallAnthias,
  SmithsAnthias,
  HawkAnthias,
  BlackstripeCardinal,
  RingtailCardinal,
  PajamaCardinal,
  FrostfinCardinal,
  RedspotCardinal,
  ThreadfinCardinal,
  FiveLineCardinal,
  OchreCardinal,
  BlueEyedCardinal,
  FragileCardinal,
  GirdledCardinal,
  LevelfinCardinal,
  SangiCardinal,
  SpotfinCardinal,
  TinyCardinal,
  BanggaiCardinal,
  BlueTang,
  YellowTang,
  ConvictTang,
  SergeantMajor,
  GoldenSweepers,
  CopperSweeper,
  HardyheadSilverside,
  BlueSprat,
  ScissortailSergeant,
  IndigoDartfish,
  FireDartfish,
  YellowtailSurgeon,
  PowderBlueSurgeon,
  OrangespineUnicorn,
  SpottedUnicorn,
  BrassyChub,
  Clownfish,
  MoorishIdol,
  GreenSeaTurtle,
  PinkStarfish,
  Moray,
  ReefOctopus,
  BlacktipReefShark,
  WhitetipReefShark,
  WhaleShark,
  ReefManta,
  HumpbackWhale,
  Chromis,
  Anemone,
  Kelp,
  FishFlake,
  Count
};

struct SpeciesInfo {
  SpeciesId id;
  const char* bodyType;
  const char* motionType;
  const char* shortName;
  const char* description;
  bool isSwimmer;
  uint8_t weight;
  SilhouetteKind silhouette;
  SizeClass sizeClass;
  uint8_t hue;
  uint8_t sat;
  uint8_t val;
};

inline constexpr uint8_t SPECIES_COUNT =
    static_cast<uint8_t>(SpeciesId::Count);

inline const SpeciesInfo& speciesInfo(SpeciesId id) {
  static const SpeciesInfo kCatalog[SPECIES_COUNT] = {
      {SpeciesId::BlueGreenChromis, "BlueGreenChromis", "Fish", "BLUE GREEN CHR",
       "SHOALS ABOVE CORAL AND FACES THE CURRENT TO EAT PLANKTON.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 150, 180, 255},
      {SpeciesId::BlackAxilChromis, "BlackAxilChromis", "Fish", "BLACK AXIL CHR",
       "A GREEN SHOALER WITH A DARK SPOT AT THE PECTORAL FIN BASE.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 145, 200, 255},
      {SpeciesId::BlackbarChromis, "BlackbarChromis", "Fish", "BLACKBAR CHR",
       "A SMALL DAMSEL WITH A DARK BAR NEAR THE TAIL.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 160, 160, 240},
      {SpeciesId::BicolorChromis, "BicolorChromis", "Fish", "BICOLOR CHR",
       "FRONT HALF DARK AND REAR HALF WHITE OVER THE REEF.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 0, 0, 230},
      {SpeciesId::AmbonChromis, "AmbonChromis", "Fish", "AMBON CHR",
       "A YELLOW BROWN SHOALER COMMON ON INDO PACIFIC REEFS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 35, 180, 240},
      {SpeciesId::TernateChromis, "TernateChromis", "Fish", "TERNATE CHR",
       "A SLENDER CHROMIS THAT FORMS TIGHT MIDWATER GROUPS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 155, 170, 250},
      {SpeciesId::WebersChromis, "WebersChromis", "Fish", "WEBERS CHR",
       "A DEEP BODIED CHROMIS OF OUTER REEF SLOPES.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 140, 150, 230},
      {SpeciesId::BarrierReefChromis, "BarrierReefChromis", "Fish", "BARRIER CHR",
       "A PACIFIC CHROMIS OF THE GREAT BARRIER REEF.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 148, 190, 255},
      {SpeciesId::DarkfinChromis, "DarkfinChromis", "Fish", "DARKFIN CHR",
       "NAMED FOR THE DARK EDGES ON ITS FINS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 130, 200, 240},
      {SpeciesId::DeepReefChromis, "DeepReefChromis", "Fish", "DEEP REEF CHR",
       "HOVERS OVER DEEPER CORAL HEADS BY DAY.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 165, 160, 230},
      {SpeciesId::YellowChromis, "YellowChromis", "Fish", "YELLOW CHR",
       "A BRIGHT YELLOW SHOALER OF CLEAR LAGOONS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 42, 230, 255},
      {SpeciesId::SpinyChromis, "SpinyChromis", "Fish", "SPINY CHR",
       "A STOCKY CHROMIS WITH A SPINY DORSAL FIN.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 120, 140, 220},
      {SpeciesId::HalfAndHalfChromis, "HalfAndHalfChromis", "Fish", "HALF HALF CHR",
       "SPLIT COLOR MAKES IT EASY TO SPOT IN A MIXED SHOAL.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 10, 50, 220},
      {SpeciesId::MidgetChromis, "MidgetChromis", "Fish", "MIDGET CHR",
       "ONE OF THE SMALLEST OPEN WATER DAMSELS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 155, 120, 240},
      {SpeciesId::OvateChromis, "OvateChromis", "Fish", "OVATE CHR",
       "A ROUND BODIED CHROMIS OF BRANCHING CORAL.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 150, 170, 245},
      {SpeciesId::PaleChromis, "PaleChromis", "Fish", "PALE CHR",
       "A PALE BLUE GREEN FISH IN LARGE CURTAINS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 140, 80, 250},
      {SpeciesId::PearlChromis, "PearlChromis", "Fish", "PEARL CHR",
       "A SILVERY CHROMIS THAT GLINTS IN SUNLIGHT.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 0, 0, 245},
      {SpeciesId::SapphireChromis, "SapphireChromis", "Fish", "SAPPHIRE CHR",
       "DEEP BLUE SIDES STAND OUT AGAINST CORAL.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 170, 220, 255},
      {SpeciesId::ScissortailChromis, "ScissortailChromis", "Fish", "SCISSOR CHR",
       "A FORKED TAIL HELPS IT DART FROM PREDATORS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 145, 190, 250},
      {SpeciesId::StaghornChromis, "StaghornChromis", "Fish", "STAGHORN CHR",
       "SHELTERS IN STAGHORN CORAL AT NIGHT.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 155, 160, 235},
      {SpeciesId::SurgeChromis, "SurgeChromis", "Fish", "SURGE CHR",
       "HOLDS IN WAVE SURGE ON THE REEF CREST.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 135, 180, 240},
      {SpeciesId::TwinspotChromis, "TwinspotChromis", "Fish", "TWINSPOT CHR",
       "TWO DARK SPOTS MARK THE REAR OF THE BODY.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 40, 200, 240},
      {SpeciesId::VanderbiltsChromis, "VanderbiltsChromis", "Fish", "VANDERBILT CHR",
       "A PACIFIC SHOALER OF CLEAR OUTER REEFS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 160, 180, 255},
      {SpeciesId::WhitetailChromis, "WhitetailChromis", "Fish", "WHITETAIL CHR",
       "A WHITE CAUDAL FIN FLASHES AS IT TURNS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 0, 0, 250},
      {SpeciesId::FusilierDamsel, "FusilierDamsel", "Fish", "FUSILIER DAMSEL",
       "A STREAMLINED DAMSEL THAT SHOALS LIKE A FUSILIER.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 150, 160, 245},
      {SpeciesId::GreenChromis, "GreenChromis", "Fish", "GREEN CHR",
       "METALLIC GREEN SIDES IN LOOSE MIDWATER GROUPS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 100, 200, 230},
      {SpeciesId::IndigoChromis, "IndigoChromis", "Fish", "INDIGO CHR",
       "DARK BLUE FISH THAT FORM DENSE CLOUDS.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 175, 210, 220},
      {SpeciesId::ReefChromis, "ReefChromis", "Fish", "REEF CHR",
       "A COMMON GENERALIST SHOALER OVER HARD CORAL.",
       true, 16, SilhouetteKind::ChromisShoal, SizeClass::Tiny, 150, 150, 240},
      {SpeciesId::BlueAndYellowFusilier, "BlueAndYellowFusilier", "Fish", "BLUE YEL FUSIL",
       "SCHOOLS IN OPEN WATER WITH A BRIGHT YELLOW BACK.",
       true, 16, SilhouetteKind::Fusilier, SizeClass::Small, 45, 220, 255},
      {SpeciesId::BluestreakFusilier, "BluestreakFusilier", "Fish", "BLUESTREAK FUS",
       "A FAST SCHOOLER WITH A BLUE LATERAL STREAK.",
       true, 16, SilhouetteKind::Fusilier, SizeClass::Small, 160, 200, 255},
      {SpeciesId::RuddyFusilier, "RuddyFusilier", "Fish", "RUDDY FUSILIER",
       "REDDISH SIDES IN TIGHT MIDWATER SCHOOLS.",
       true, 16, SilhouetteKind::Fusilier, SizeClass::Small, 5, 200, 230},
      {SpeciesId::GoldbandFusilier, "GoldbandFusilier", "Fish", "GOLDBAND FUS",
       "A GOLD BAND RUNS THE LENGTH OF THE BODY.",
       true, 16, SilhouetteKind::Fusilier, SizeClass::Small, 40, 230, 250},
      {SpeciesId::DoubleLinedFusilier, "DoubleLinedFusilier", "Fish", "DOUBLE LINE FUS",
       "TWO STRIPES MARK THIS ACTIVE SCHOOLER.",
       true, 16, SilhouetteKind::Fusilier, SizeClass::Small, 35, 180, 245},
      {SpeciesId::YellowbackFusilier, "YellowbackFusilier", "Fish", "YELBACK FUS",
       "YELLOW ABOVE AND BLUE BELOW IN LARGE SCHOOLS.",
       true, 16, SilhouetteKind::Fusilier, SizeClass::Small, 48, 220, 255},
      {SpeciesId::LunarFusilier, "LunarFusilier", "Fish", "LUNAR FUSILIER",
       "A SILVERY FUSILIER OF CLEAR TROPICAL SEAS.",
       true, 16, SilhouetteKind::Fusilier, SizeClass::Small, 0, 0, 240},
      {SpeciesId::MojicaFusilier, "MojicaFusilier", "Fish", "MOJICA FUSILIER",
       "A PACIFIC FUSILIER OF OUTER REEF WALLS.",
       true, 16, SilhouetteKind::Fusilier, SizeClass::Small, 150, 170, 245},
      {SpeciesId::StriatedFusilier, "StriatedFusilier", "Fish", "STRIATED FUS",
       "FINE STRIPES COVER A STREAMLINED BODY.",
       true, 16, SilhouetteKind::Fusilier, SizeClass::Small, 30, 160, 235},
      {SpeciesId::WideBandFusilier, "WideBandFusilier", "Fish", "WIDEBAND FUS",
       "A BROAD COLOR BAND HELPS THE SCHOOL STAY TIGHT.",
       true, 16, SilhouetteKind::Fusilier, SizeClass::Small, 50, 200, 250},
      {SpeciesId::LyretailAnthias, "LyretailAnthias", "Fish", "LYRETAIL ANTH",
       "A PINK ORANGE SHOALER WITH A LYRE SHAPED TAIL.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 5, 220, 255},
      {SpeciesId::ThreadfinAnthias, "ThreadfinAnthias", "Fish", "THREADFIN ANTH",
       "LONG THREAD LIKE FINS STREAM FROM THE BODY.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 10, 200, 250},
      {SpeciesId::PurpleAnthias, "PurpleAnthias", "Fish", "PURPLE ANTHIAS",
       "PURPLE AND YELLOW FISH IN LOOSE SHOALS.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 200, 180, 255},
      {SpeciesId::BartlettsAnthias, "BartlettsAnthias", "Fish", "BARTLETT ANTH",
       "A PACIFIC ANTHIAS WITH A BRIGHT ORANGE BODY.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 15, 230, 255},
      {SpeciesId::SquarebackAnthias, "SquarebackAnthias", "Fish", "SQUAREBACK ANT",
       "A SQUARED TAIL MARKS THIS REEF SHOALER.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 8, 210, 245},
      {SpeciesId::FlameAnthias, "FlameAnthias", "Fish", "FLAME ANTHIAS",
       "A FIERY RED ORANGE FISH OF CURRENT SWEPT REEFS.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 0, 240, 255},
      {SpeciesId::ResplendentAnthias, "ResplendentAnthias", "Fish", "RESPLEND ANTH",
       "BRIGHT COLORS HELP MALES HOLD A HAREM.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 20, 220, 255},
      {SpeciesId::RedfinAnthias, "RedfinAnthias", "Fish", "REDFIN ANTHIAS",
       "RED FINS FLASH AS THE SHOAL TURNS.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 5, 230, 250},
      {SpeciesId::YellowbackAnthias, "YellowbackAnthias", "Fish", "YELBACK ANTH",
       "YELLOW ON THE BACK OVER A PINK BODY.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 40, 200, 250},
      {SpeciesId::PrincessAnthias, "PrincessAnthias", "Fish", "PRINCESS ANTH",
       "A SLENDER ANTHIAS OF DEEP REEF SLOPES.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 200, 160, 245},
      {SpeciesId::SunshineAnthias, "SunshineAnthias", "Fish", "SUNSHINE ANTH",
       "YELLOW ORANGE FISH THAT HOVER IN CURRENT.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 35, 230, 255},
      {SpeciesId::DisparAnthias, "DisparAnthias", "Fish", "DISPAR ANTHIAS",
       "A SMALL PACIFIC ANTHIAS IN MIXED SHOALS.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 10, 180, 240},
      {SpeciesId::EvansiAnthias, "EvansiAnthias", "Fish", "EVANS ANTHIAS",
       "A DEEP WATER ANTHIAS WITH A LONG DORSAL FIN.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 15, 200, 245},
      {SpeciesId::LoriAnthias, "LoriAnthias", "Fish", "LORI ANTHIAS",
       "A COMPACT ANTHIAS OF CORAL RICH SLOPES.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 5, 210, 250},
      {SpeciesId::PasculAnthias, "PasculAnthias", "Fish", "PASCUL ANTHIAS",
       "A SOUTH PACIFIC SHOALER ABOVE CORAL HEADS.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 12, 190, 245},
      {SpeciesId::RandallAnthias, "RandallAnthias", "Fish", "RANDALL ANTH",
       "NAMED AFTER A FAMED REEF FISH TAXONOMIST.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 8, 220, 250},
      {SpeciesId::SmithsAnthias, "SmithsAnthias", "Fish", "SMITH ANTHIAS",
       "A STOCKY ANTHIAS OF INDIAN OCEAN REEFS.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 18, 200, 245},
      {SpeciesId::HawkAnthias, "HawkAnthias", "Fish", "HAWK ANTHIAS",
       "A SWIFT ANTHIAS THAT DARTS AT PLANKTON.",
       true, 8, SilhouetteKind::Anthias, SizeClass::Tiny, 0, 200, 240},
      {SpeciesId::BlackstripeCardinal, "BlackstripeCardinal", "Fish", "BLACKSTRIPE CD",
       "DARK STRIPES RUN THE LENGTH OF A SMALL BODY.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 0, 0, 220},
      {SpeciesId::RingtailCardinal, "RingtailCardinal", "Fish", "RINGTAIL CARD",
       "A GOLD RING MARKS THE BASE OF THE TAIL.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 35, 200, 240},
      {SpeciesId::PajamaCardinal, "PajamaCardinal", "Fish", "PAJAMA CARD",
       "SPOTS AND BARS LOOK LIKE SLEEPWEAR.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 200, 100, 230},
      {SpeciesId::FrostfinCardinal, "FrostfinCardinal", "Fish", "FROSTFIN CARD",
       "PALE FINS EDGE A REDDISH BODY.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 5, 180, 230},
      {SpeciesId::RedspotCardinal, "RedspotCardinal", "Fish", "REDSPOT CARD",
       "A RED SPOT SITS NEAR THE BASE OF THE TAIL.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 0, 200, 230},
      {SpeciesId::ThreadfinCardinal, "ThreadfinCardinal", "Fish", "THREADFIN CARD",
       "LONG FILAMENTS TRAIL FROM THE FINS.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 10, 160, 235},
      {SpeciesId::FiveLineCardinal, "FiveLineCardinal", "Fish", "FIVE LINE CARD",
       "FIVE STRIPES MAKE IT EASY TO COUNT IN A GROUP.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 20, 150, 230},
      {SpeciesId::OchreCardinal, "OchreCardinal", "Fish", "OCHRE CARDINAL",
       "A WARM BROWN CARDINAL OF LAGOON CORAL.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 25, 180, 220},
      {SpeciesId::BlueEyedCardinal, "BlueEyedCardinal", "Fish", "BLUE EYE CARD",
       "A BRIGHT BLUE EYE STANDS OUT AT DUSK.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 160, 100, 240},
      {SpeciesId::FragileCardinal, "FragileCardinal", "Fish", "FRAGILE CARD",
       "A SLENDER CARDINAL THAT HIDES BY DAY.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 0, 80, 230},
      {SpeciesId::GirdledCardinal, "GirdledCardinal", "Fish", "GIRDLED CARD",
       "A DARK GIRDLE CROSSES THE MID BODY.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 15, 140, 220},
      {SpeciesId::LevelfinCardinal, "LevelfinCardinal", "Fish", "LEVELFIN CARD",
       "EVEN FINS SUIT A QUIET HOVERING HABIT.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 30, 120, 225},
      {SpeciesId::SangiCardinal, "SangiCardinal", "Fish", "SANGI CARDINAL",
       "A CARDINAL OF INDONESIAN REEF CAVES.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 5, 160, 230},
      {SpeciesId::SpotfinCardinal, "SpotfinCardinal", "Fish", "SPOTFIN CARD",
       "A DARK SPOT MARKS THE DORSAL FIN.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 10, 170, 235},
      {SpeciesId::TinyCardinal, "TinyCardinal", "Fish", "TINY CARDINAL",
       "ONE OF THE SMALLEST REEF CARDINALS.",
       true, 8, SilhouetteKind::Cardinal, SizeClass::Tiny, 0, 100, 240},
      {SpeciesId::BanggaiCardinal, "BanggaiCardinal", "Fish", "BANGGAI CARD",
       "A RARE CARDINAL FROM A SMALL INDONESIAN RANGE.",
       true, 2, SilhouetteKind::Cardinal, SizeClass::Tiny, 0, 0, 245},
      {SpeciesId::BlueTang, "BlueTang", "Fish", "BLUE TANG",
       "A SURGEONFISH THAT GRAZES ALGAE IN LOOSE GROUPS.",
       true, 4, SilhouetteKind::BlueTang, SizeClass::Medium, 160, 220, 255},
      {SpeciesId::YellowTang, "YellowTang", "Fish", "YELLOW TANG",
       "A REEF HERBIVORE THAT CROPS ALGAE WITH OTHERS.",
       true, 4, SilhouetteKind::YellowTang, SizeClass::Medium, 40, 240, 255},
      {SpeciesId::ConvictTang, "ConvictTang", "Fish", "CONVICT TANG",
       "BLACK BARS CROSS A SILVER BODY IN LARGE GRAZING GROUPS.",
       true, 4, SilhouetteKind::Sergeant, SizeClass::Medium, 0, 0, 240},
      {SpeciesId::SergeantMajor, "SergeantMajor", "Fish", "SERGEANT MAJOR",
       "BOLD BARS MARK A DAMSEL THAT FORMS LOOSE GROUPS.",
       true, 4, SilhouetteKind::Sergeant, SizeClass::Small, 40, 200, 255},
      {SpeciesId::GoldenSweepers, "GoldenSweeper", "Fish", "GOLDEN SWEEPER",
       "DENSE SCHOOLS FILL CAVES BY DAY.",
       true, 4, SilhouetteKind::Sweeper, SizeClass::Tiny, 40, 220, 250},
      {SpeciesId::CopperSweeper, "CopperSweeper", "Fish", "COPPER SWEEPER",
       "COPPER SIDES GLINT IN CAVE MOUTH SCHOOLS.",
       true, 4, SilhouetteKind::Sweeper, SizeClass::Tiny, 20, 180, 230},
      {SpeciesId::HardyheadSilverside, "Hardyhead", "Fish", "HARDYHEAD",
       "SILVERY FISH THAT FORM TRUE POLARIZED SCHOOLS.",
       true, 4, SilhouetteKind::Silverside, SizeClass::Tiny, 0, 0, 245},
      {SpeciesId::BlueSprat, "BlueSprat", "Fish", "BLUE SPRAT",
       "A SMALL HERRING LIKE SCHOOLER OF LAGOONS.",
       true, 4, SilhouetteKind::Silverside, SizeClass::Tiny, 160, 100, 250},
      {SpeciesId::ScissortailSergeant, "ScissorSergeant", "Fish", "SCISSOR SGT",
       "A FORKED TAIL AND BARS MARK THIS GROUP DAMSEL.",
       true, 4, SilhouetteKind::Sergeant, SizeClass::Small, 35, 190, 250},
      {SpeciesId::IndigoDartfish, "IndigoDart", "Fish", "INDIGO DART",
       "PAIRS OR SMALL GROUPS HOVER ABOVE BURROWS.",
       true, 4, SilhouetteKind::Dartfish, SizeClass::Tiny, 170, 200, 240},
      {SpeciesId::FireDartfish, "FireDart", "Fish", "FIRE DARTFISH",
       "AN ORANGE BODY AND WHITE TAIL OVER SAND.",
       true, 4, SilhouetteKind::Dartfish, SizeClass::Tiny, 10, 230, 255},
      {SpeciesId::YellowtailSurgeon, "YelTailSurgeon", "Fish", "YEL TAIL SURG",
       "A DARK SURGEON WITH A BRIGHT YELLOW TAIL.",
       true, 4, SilhouetteKind::Surgeon, SizeClass::Medium, 160, 180, 220},
      {SpeciesId::PowderBlueSurgeon, "PowderSurgeon", "Fish", "POWDER SURG",
       "A PALE BLUE SURGEON THAT GRAZES IN LOOSE GROUPS.",
       true, 4, SilhouetteKind::Surgeon, SizeClass::Medium, 155, 200, 255},
      {SpeciesId::OrangespineUnicorn, "OrangeUnicorn", "Fish", "ORANGE UNICORN",
       "A UNICORNFISH THAT JOINS GRAZING GROUPS.",
       true, 4, SilhouetteKind::Unicorn, SizeClass::Medium, 25, 200, 240},
      {SpeciesId::SpottedUnicorn, "SpotUnicorn", "Fish", "SPOT UNICORN",
       "SPOTS COVER A LARGE GRAZING UNICORNFISH.",
       true, 4, SilhouetteKind::Unicorn, SizeClass::Medium, 0, 40, 230},
      {SpeciesId::BrassyChub, "BrassyChub", "Fish", "BRASSY CHUB",
       "A BRASSY SCHOOLER OF THE REEF SURF ZONE.",
       true, 4, SilhouetteKind::Sergeant, SizeClass::Small, 35, 150, 220},
      {SpeciesId::Clownfish, "Clownfish", "Fish", "CLOWNFISH",
       "LIVES IN A HOST ANEMONE. MUCUS BLOCKS THE STING.",
       true, 3, SilhouetteKind::Clownfish, SizeClass::Small, 18, 230, 255},
      {SpeciesId::MoorishIdol, "MoorishIdol", "Fish", "MOORISH IDOL",
       "A LONG SNOUT PICKS SPONGES. THE STREAMER IS A DORSAL FIN.",
       true, 3, SilhouetteKind::MoorishIdol, SizeClass::Medium, 0, 0, 245},
      {SpeciesId::GreenSeaTurtle, "Turtle", "Turtle", "SEA TURTLE",
       "ADULTS GRAZE SEAGRASS. FEMALES RETURN HOME TO NEST.",
       true, 3, SilhouetteKind::Turtle, SizeClass::Large, 90, 150, 200},
      {SpeciesId::PinkStarfish, "Star", "Star", "STARFISH",
       "NO BRAIN AND NO BLOOD. TUBE FEET WALK ON ROCK.",
       true, 3, SilhouetteKind::Star, SizeClass::Medium, 230, 180, 255},
      {SpeciesId::Moray, "Snake", "Snake", "MORAY",
       "A CREVICE HUNTER WITH A SECOND JAW IN THE THROAT.",
       true, 1, SilhouetteKind::Snake, SizeClass::Large, 80, 180, 200},
      {SpeciesId::ReefOctopus, "Octopus", "Octopus", "OCTOPUS",
       "EIGHT ARMS AND THREE HEARTS. SKIN SHIFTS COLOR TO VANISH.",
       true, 1, SilhouetteKind::Octopus, SizeClass::Large, 200, 180, 220},
      {SpeciesId::BlacktipReefShark, "BlacktipShark", "Fish", "BLACKTIP SHARK",
       "A REEF SHARK WITH BLACK FIN TIPS. IT PATROLS SHALLOW FLATS.",
       true, 2, SilhouetteKind::Shark, SizeClass::Large, 0, 0, 180},
      {SpeciesId::WhitetipReefShark, "WhitetipShark", "Fish", "WHITETIP SHARK",
       "A SLENDER REEF SHARK THAT RESTS IN CAVES BY DAY.",
       true, 2, SilhouetteKind::Shark, SizeClass::Large, 0, 20, 200},
      {SpeciesId::WhaleShark, "WhaleShark", "Fish", "WHALE SHARK",
       "THE LARGEST FISH. IT FILTERS PLANKTON ON LONG MIGRATIONS.",
       true, 1, SilhouetteKind::WhaleShark, SizeClass::Large, 0, 0, 220},
      {SpeciesId::ReefManta, "Manta", "Fish", "REEF MANTA",
       "A WIDE WINGED RAY THAT LOOPS THROUGH PLANKTON CLOUDS.",
       true, 1, SilhouetteKind::Manta, SizeClass::Large, 170, 80, 230},
      {SpeciesId::HumpbackWhale, "Humpback", "Turtle", "HUMPBACK",
       "A LONG LIVED WHALE. SONGS AND FLIPPERS MARK THE SPECIES.",
       true, 1, SilhouetteKind::Whale, SizeClass::Large, 0, 40, 160},
      {SpeciesId::Chromis, "Chromis", "Chromis", "CHROMIS",
       "TINY BLUE SCHOOLERS RENDERED AS BACKGROUND DOTS.",
       false, 0, SilhouetteKind::Prop, SizeClass::Tiny, 160, 180, 255},
      {SpeciesId::Anemone, "Anemone", "Anemone", "ANEMONE",
       "AN ANIMAL POLYP. STINGING CELLS CATCH PREY.",
       false, 0, SilhouetteKind::Prop, SizeClass::Small, 0, 150, 220},
      {SpeciesId::Kelp, "Kelp", "Kelp", "KELP",
       "A BROWN ALGA. BLADES MAKE FOOD FROM SUNLIGHT.",
       false, 0, SilhouetteKind::Prop, SizeClass::Medium, 90, 200, 180},
      {SpeciesId::FishFlake, "FishFlake", "FishFlake", "FISH FLAKE",
       "PRESSED FLAKES THAT SINK AND FEED THE SWIMMERS.",
       false, 0, SilhouetteKind::Prop, SizeClass::Tiny, 30, 220, 255},
  };
  const uint8_t index = static_cast<uint8_t>(id);
  if (index >= SPECIES_COUNT) {
    return kCatalog[0];
  }
  return kCatalog[index];
}

inline SpeciesId speciesIdFromBodyType(const String& bodyType) {
  if (bodyType == "Clownfish") return SpeciesId::Clownfish;
  if (bodyType == "BlueTang") return SpeciesId::BlueTang;
  if (bodyType == "YellowTang") return SpeciesId::YellowTang;
  if (bodyType == "MoorishIdol") return SpeciesId::MoorishIdol;
  if (bodyType == "Turtle") return SpeciesId::GreenSeaTurtle;
  if (bodyType == "Star") return SpeciesId::PinkStarfish;
  if (bodyType == "Snake") return SpeciesId::Moray;
  if (bodyType == "Octopus") return SpeciesId::ReefOctopus;
  if (bodyType == "Chromis") return SpeciesId::Chromis;
  if (bodyType == "Anemone") return SpeciesId::Anemone;
  if (bodyType == "Kelp") return SpeciesId::Kelp;
  if (bodyType == "FishFlake") return SpeciesId::FishFlake;
  if (bodyType == "Food") return SpeciesId::FishFlake;
  if (bodyType == "Fish") return SpeciesId::Clownfish;
  for (uint8_t i = 0; i < SPECIES_COUNT; ++i) {
    if (bodyType == speciesInfo(static_cast<SpeciesId>(i)).bodyType) {
      return static_cast<SpeciesId>(i);
    }
  }
  return SpeciesId::Clownfish;
}

inline bool isFishMotionSpecies(SpeciesId id) {
  const char* motion = speciesInfo(id).motionType;
  return strcmp(motion, "Fish") == 0;
}

inline bool isSwimmerSpecies(SpeciesId id) {
  return speciesInfo(id).isSwimmer;
}

inline SpeciesId pickWeightedSwimmer(const bool* exclude) {
  uint16_t total = 0;
  for (uint8_t i = 0; i < SPECIES_COUNT; ++i) {
    const SpeciesInfo& info = speciesInfo(static_cast<SpeciesId>(i));
    if (!info.isSwimmer || info.weight == 0) continue;
    if (exclude != nullptr && exclude[i]) continue;
    total += info.weight;
  }
  if (total == 0) {
    return SpeciesId::BlueGreenChromis;
  }
  uint16_t roll = static_cast<uint16_t>(random(total));
  for (uint8_t i = 0; i < SPECIES_COUNT; ++i) {
    const SpeciesInfo& info = speciesInfo(static_cast<SpeciesId>(i));
    if (!info.isSwimmer || info.weight == 0) continue;
    if (exclude != nullptr && exclude[i]) continue;
    if (roll < info.weight) {
      return static_cast<SpeciesId>(i);
    }
    roll -= info.weight;
  }
  return SpeciesId::BlueGreenChromis;
}

inline SpeciesId randomSwimmerSpecies() {
  return pickWeightedSwimmer(nullptr);
}

inline void pickWeightedRoster(SpeciesId* out, uint8_t count) {
  bool used[SPECIES_COUNT] = {};
  for (uint8_t n = 0; n < count; ++n) {
    const SpeciesId id = pickWeightedSwimmer(used);
    out[n] = id;
    used[static_cast<uint8_t>(id)] = true;
  }
}

inline float sizeClassScale(SizeClass sizeClass) {
  switch (sizeClass) {
    case SizeClass::Tiny:
      return 0.55f;
    case SizeClass::Small:
      return 0.75f;
    case SizeClass::Large:
      return 1.25f;
    case SizeClass::Medium:
    default:
      return 1.0f;
  }
}

#endif  // SPECIES_CATALOG_H
