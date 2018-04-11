package FiftyOneDegreesPatternV3

import (
    "time"
    "testing"
    "encoding/csv"
    "os"
    "io"
    "log"
)

/*
 * Constants
 */

// Lite data file location.
const LITE_DATA_FILE = "../../../data/51Degrees-LiteV3.2.dat"
// User-Agents file location
const USER_AGENTS_FILE = "../../../data/20000 User Agents.csv"

// User-Agent string of an iPhone mobile device.
const MOBILE_USER_AGENT = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS" +
    " X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167" + 
    " Safari/9537.53"
// User-Agent string of Firefox Web browser version 41 on desktop.
const DESKTOP_USER_AGENT = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0)" + 
    " Gecko/20100101 Firefox/41.0"
// User-Agent string of a MediaHub device.
const MEDIA_HUB_USER_AGENT = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core" +
    " Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0" + 
    " Chrome/30.0.0.0 Safari/537.36"

// Number of User-Agents to load from the source data file.
var NUM_RECORDS = 1000

/*
 * Fields
 */

// Read the User Agents file into a slice.
var userAgents = GetRecords()

// Array of known properties used to test Available properties
var properties = []string{"IsMobile","BrowserName"}

// Time used to compare times with reload.
var baseTime int64

// Initialise the Provider.
var provider = NewProvider(LITE_DATA_FILE)

/*
 * Utils
 */

// Get records from the source User-Agents file.
func GetRecords() []string {
    var tempRecords []string
    
    // Set csv up reader
    fin, err := os.Open(USER_AGENTS_FILE)
    
    if err != nil{
        log.Fatal(err)
    }
    
    r := csv.NewReader(fin)
    
    // Separator between userAgents in the input csv file is a pipe, this is 
    // because User-Agents can contain commas.
    r.Comma = '|'
    r.LazyQuotes = true
    
    for i := 0; i < NUM_RECORDS; i++ {
	    record, err := r.Read()
	    
	    // Stop at EOF.
	    if err == io.EOF {
		    break
	    }
	    
	    // The first part of each record is the User-Agent, append it to
	    // userAgents array.
	    tempRecords = append(tempRecords, record[0])
    }
    
    return tempRecords
}

// Convert a vector string into an iterable slice.
func VectorStringToSlice(vector VectorString) []string{
    var tempSlice []string
    
    for i := 0; i < int(vector.Size()); i++ {
        tempSlice = append(tempSlice, vector.Get(i))
    }
    
    return tempSlice
}

// Get the index of an item within a string slice.
func SliceIndexOf(input []string, key string) int {
    for i := 0; i < len(input); i++ {
        if input[i] == key {
            return i
        }
    }
    return -1
}

// Match User-Agents function used in testing.
func MatchAllUserAgents() int {
    for _, userAgent := range userAgents { 
        match := provider.GetMatch(userAgent)
        if match == nil  {
            DeleteMatch(match)
            return 1
        }
        DeleteMatch(match)
    }
    return 0
}

/*
 * Tests
 */

func TestMobileUserAgent(t *testing.T) {
    match := provider.GetMatch(MOBILE_USER_AGENT)
    
    if match == nil {
        t.Fatalf("Should return a valid match object")
    }
    expectedResult := "True"
    actualResult := match.GetValue("IsMobile")
    
    if actualResult != expectedResult {
        t.Fatalf("Expected %s but got %s", expectedResult, actualResult)
    }
    
    DeleteMatch(match)
}

func TestDesktopUserAgent(t *testing.T) {
    match := provider.GetMatch(DESKTOP_USER_AGENT)
    
    if match == nil {
        t.Fatalf("Should return a valid match object")
    }
    expectedResult := "False"
    actualResult := match.GetValue("IsMobile")
    
    if actualResult != expectedResult {
        t.Fatalf("Expected %s but got %s", expectedResult, actualResult)
    }
    
    DeleteMatch(match)
}

func TestMediaHubUserAgent(t *testing.T) {
    match := provider.GetMatch(MEDIA_HUB_USER_AGENT)
    
    if match == nil {
        t.Fatalf("Should return a valid match object")
    }
    expectedResult := "False"
    actualResult := match.GetValue("IsMobile")
    
    if actualResult != expectedResult {
        t.Fatalf("Expected %s but got %s", expectedResult, actualResult)
    }
    
    DeleteMatch(match)
}

func TestAllAvailableProperties (t *testing.T) {
    for _, userAgent := range userAgents {
        match := provider.GetMatch(userAgent)
        if match == nil {
            t.Fatalf("Should return a valid match object")
        }
        if match.GetValue("isMobile") == "True" {
            for _, property := range VectorStringToSlice(
                provider.GetAvailableProperties()) {
                if len([]rune(match.GetValue(property))) == 0  {
                    t.Fatalf("Value of %s returned was empty for User-Agent:" +
                        " %s",property, userAgent)
                }
            }
        }
        DeleteMatch(match)
    }
}

func TestUnavailableProperties (t *testing.T) {
    for _, userAgent := range userAgents {
        match := provider.GetMatch(userAgent)
        if match == nil {
            t.Fatalf("Should return a valid match object")
        }

        if len([]rune(match.GetValue("notaproperty"))) > 0 {
            t.Fatalf("Match returned %s for nonexistant property", 
                match.GetValue("notaproperty"))
        }
        
        DeleteMatch(match)
    }
}


func TestMatchForDeviceID (t *testing.T) {
    for _, userAgent := range userAgents {
        match := provider.GetMatch(userAgent)
        if match == nil {
            t.Fatalf("GetMatch should return a valid match object")
        }
        
        deviceId := match.GetDeviceId()
        deviceMatch := provider.GetMatchForDeviceId(string(deviceId))

        if deviceMatch == nil {
            t.Fatalf("GetMatchForDeviceId should return a valid match object")
        }
        
        for _, property := range VectorStringToSlice(
            provider.GetAvailableProperties()) {
            if match.GetValue(property) != deviceMatch.GetValue(property) {
                t.Fatalf("DeviceId: '%s', Match for property '%s' from " + 
                    "matched Device Id did not match the original match" + 
                    " for User-Agent: %s", deviceId, property, userAgent)
            }
        }
        DeleteMatch(match)
        DeleteMatch(deviceMatch)
    }
}


func TestFindMobileProfiles(t *testing.T) {
    profiles := provider.FindProfiles("IsMobile", "True")
    for i := 0; i < profiles.GetCount(); i++ {
        match := provider.GetMatchForDeviceId(string(profiles.GetProfileId(i)))
        if match == nil {
            t.Fatalf("GetMatchForDeviceId should return a valid match object")
        }
        if match.GetValue("isMobile") == "False" {
            t.Fatalf("Profile with profile id: %d is not a mobile profile", 
                profiles.GetProfileId(i))
        }
        DeleteMatch(match)
    }
}

func TestFindNonMobileProfiles(t *testing.T) {
    profiles := provider.FindProfiles("IsMobile", "False")
    for i := 0; i < profiles.GetCount(); i++ {
        match := provider.GetMatchForDeviceId(string(profiles.GetProfileId(i)))
        if match == nil {
            t.Fatalf("GetMatchForDeviceId should return a valid match object")
        }
        if match.GetValue("IsMobile") == "True" {
            t.Fatalf("Profile with profile id: %d is a mobile profile", 
                profiles.GetProfileId(i))
        }
        DeleteMatch(match)
    }
}

/*
 * Helper Method tests
 */

func TestMatchOverloadMethods(t *testing.T) {
    var headers MapStringString = NewMapStringString()
    headers.Set("User-Agent", MOBILE_USER_AGENT)
    
    var match Match

    match = provider.GetMatch(MOBILE_USER_AGENT)
    if match.GetValue("IsMobile") == "False" {
        t.Fatalf("Should return User-Agent matches correctly: %s", 
            MOBILE_USER_AGENT)
    }

    DeleteMatch(match)

    match = provider.GetMatch(headers)

    if match.GetValue("IsMobile") == "False" {
        t.Fatalf("Should return HTTP header matches correctly")
    }

    DeleteMatch(match)
} 

func TestMatchingFromFindProfiles(t *testing.T) {
    profiles := provider.FindProfiles("IsMobile", "True")
    
    for i := 0; i < profiles.GetCount(); i++ {
        match := provider.GetMatchForDeviceId(string(profiles.GetProfileId(i)))
        if match.GetValue("IsMobile") == "False" {
            t.Fatalf("Should return a match object with " +
                "profiles.GetProfileIndex()")
        }
        DeleteMatch(match)        
    }
    
    match := provider.GetMatchForDeviceId(
        string(profiles.GetProfileId(profiles.GetCount())))
    if match.GetValue("IsMobile") != "" {
        t.Fatalf("Should return nil for an out of range profile")
    }
    DeleteMatch(match)
    
    match = provider.GetMatchForDeviceId(string(profiles.GetProfileId(0)))
    if match.GetValue("BrowserName") != "" {
        t.Fatalf("Should return nil for a property from a different component.")
    }
    DeleteMatch(match)
}

func TestAvailableProperties(t *testing.T) {
    var _properties = VectorStringToSlice(provider.GetAvailableProperties())
   
    for _, property := range properties {
        if SliceIndexOf(_properties, property) == -1 {
            t.Fatalf("Should have an array of valid propeties.")
        }
    } 
}

/*
 * Performance Tests
 */

func TestStartup(t *testing.T) {
    start := time.Now()
    
    perfProvider := NewProvider(LITE_DATA_FILE)
    
    elapsed := int64(time.Since(start) / time.Millisecond)
    
    if(elapsed > 150){
        t.Fatalf("Start up time took %d ms, Limit is 150ms.", elapsed)
    }
    
    DeleteProvider(perfProvider)
}

func TestDetectionSpeed(t *testing.T) {
    start := time.Now()
    
    if MatchAllUserAgents() == 1 {
        t.Fatalf("Should return a valid match object")
    }
        
    elapsed := int64(time.Since(start) / time.Millisecond)
    
    baseTime = elapsed
    
    var timePerDetection = float64(elapsed / int64(NUM_RECORDS))
    var detectionsPerSeconds = float64(1000 / timePerDetection)
    
    if timePerDetection > 0.1 {
        t.Fatalf("Time per detection was %f ms, max is 0.1 ms. Detections per" +
            " second: %f", timePerDetection, detectionsPerSeconds)
    }
}

func TestReloadPenalty(t *testing.T) {
    numberOfReloads := 5
    
    start := time.Now()
    
    for i := 5; i < numberOfReloads; i++ {
        MatchAllUserAgents()
        provider.ReloadFromFile();
    }
    
    elapsed := int64(time.Since(start) / time.Millisecond) 
    
    difference := elapsed - baseTime
    
    reloadPenalty := difference / int64(numberOfReloads)
    
    if reloadPenalty > 1000 {
         t.Fatalf("Time penalty for reloading was %d, max is 1000.", 
            reloadPenalty)
    }
}

/*
 * Robustness Tests
 *
 * Tests creating and reloading the provider. Tests consistency of provider.
 */

func TestNewProvider(t *testing.T) {
    _provider := NewProvider(LITE_DATA_FILE)
	actualResult := _provider.GetDataSetName()

	var expectedResult = "Lite"

	if actualResult != expectedResult {
		t.Fatalf("Expected %s but got %s", expectedResult, actualResult)
	}
	DeleteProvider(_provider)
}

func TestReloadFromFile(t *testing.T) {
	_provider := NewProvider(LITE_DATA_FILE)
	
	var expectedResult = _provider.GetDataSetDeviceCombinations()
	_provider.ReloadFromFile()
	
	var actualResult = _provider.GetDataSetDeviceCombinations()

	if actualResult != expectedResult {
		t.Fatalf("Expected %d but got %d", expectedResult, actualResult)
	}
	DeleteProvider(_provider)	
}

func TestProviderOverMultipleIterations(t *testing.T) {
    var expectedResult = "True"

    _provider := NewProvider(LITE_DATA_FILE)
    for i := 0; i < 100000; i++ {
    	match := _provider.GetMatch(MOBILE_USER_AGENT)
    	actualResult := match.GetValue("IsMobile");
    	if (actualResult != expectedResult){
    	    t.Fatalf("Expected %s but got %s", expectedResult, actualResult)
    	}
    	DeleteMatch(match)
    }
    DeleteProvider(_provider)
}

/*
 * Benchmarks
 *
 * Run using `$ go test -bench=.`
 */

func BenchmarkMemUse(b *testing.B) {
    b.ReportAllocs()
    b.ResetTimer()
    provider :=	NewProvider(LITE_DATA_FILE)
    for i := 0; i < b.N; i++ {
        match := provider.GetMatch(MOBILE_USER_AGENT)
    	match.GetValue("IsMobile");
    	DeleteMatch(match)
    }
    b.StopTimer()
}
