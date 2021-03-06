#if !defined( _AUDIO_CONFIG_H )
#define _AUDIO_CONFIG_H

/****************************************************************
 *input stream parameters:
    *sample rate: 16,000Hz
    *channel    : mono
    *bit depth  : 16
    *signness   : signed
 ****************************************************************/
#define SAMPLE_RATE      16000
#define CHANNEL_NUM      1
#define BYTES_PER_SAMPLE 2



/****************************************************************
 * Front End
 ****************************************************************/
// framing
#define FRAME_LEN          512 // 32ms
#define OVERLAP_RATIO_Q16  ( (int)(0.5 * 65536) )
#define FRAME_STRIDE       ( FRAME_LEN * ( 65536 - OVERLAP_RATIO_Q16 ) / 65536 )

// VAD
#define VAD_TYPE AUD_VAD_ENERGY

#if (VAD_TYPE == AUD_VAD_ENERGY)

	#define VAD_FRAME_LEN                 FRAME_LEN
	#define NONSPEECH_TO_SPEECH_SMOOTH    ((AUD_Int32s)(0.20 * SAMPLE_RATE / VAD_FRAME_LEN)) // unit: frames
	#define NONSPEECH_TO_SPEECH_THRESHOLD ((AUD_Int32s)(0.7 * NONSPEECH_TO_SPEECH_SMOOTH))

	#define SPEECH_TO_NONSPEECH_SMOOTH    ((AUD_Int32s)(0.32 * SAMPLE_RATE / VAD_FRAME_LEN)) // unit: frames
	#define SPEECH_TO_NONSPEECH_THRESHOLD ((AUD_Int32s)(0.7 * SPEECH_TO_NONSPEECH_SMOOTH))

	#define SMOOTH_WIN_SIZE               SPEECH_TO_NONSPEECH_SMOOTH                         // unit: frames

	#define BACKTRACK_SIZE                ((AUD_Int32s)(0.32 * SAMPLE_RATE / VAD_FRAME_LEN)) // unit: frames

	#define ALERT_CONTEXT_SIZE            ((AUD_Int32s)(0.50 * SAMPLE_RATE / VAD_FRAME_LEN)) // unit: frames
	#define ALERT_CONTEXT_THRESHOLD       ((AUD_Int32s)(0.75 * ALERT_CONTEXT_SIZE))

	#define VAD_ENERGY_INITIAL            0.004
	#define LAMBDA_L                      0.998
	#define LAMBDA_H                      0.998

#elif (VAD_TYPE == AUD_VAD_GMM)

	#define VAD_FRAME_LEN                 FRAME_STRIDE
	#define NONSPEECH_TO_SPEECH_SMOOTH    ((AUD_Int32s)(0.128 * SAMPLE_RATE / VAD_FRAME_LEN)) // unit: frames
	#define NONSPEECH_TO_SPEECH_THRESHOLD ((AUD_Int32s)(0.7 * NONSPEECH_TO_SPEECH_SMOOTH))

	#define SPEECH_TO_NONSPEECH_SMOOTH    ((AUD_Int32s)(0.256 * SAMPLE_RATE / VAD_FRAME_LEN)) // unit: frames
	#define SPEECH_TO_NONSPEECH_THRESHOLD ((AUD_Int32s)(0.7 * SPEECH_TO_NONSPEECH_SMOOTH))

	#define SMOOTH_WIN_SIZE               SPEECH_TO_NONSPEECH_SMOOTH                          // unit: frames

	#define BACKTRACK_SIZE                ((AUD_Int32s)(0.48 * SAMPLE_RATE / VAD_FRAME_LEN))  // unit: frames

	#define ALERT_CONTEXT_SIZE            ((AUD_Int32s)(0.16 * SAMPLE_RATE / VAD_FRAME_LEN))  // unit: frames
	#define ALERT_CONTEXT_THRESHOLD       ((AUD_Int32s)(0.7 * ALERT_CONTEXT_SIZE))            // for babble noise, it need to be shorter

	#if defined( ANDROID )
	#define VAD_SPEECH_MODEL_FILE	      "/sdcard/vad_model/speech.gmm"
	#define VAD_BACKGROUND_MODEL_FILE     "/sdcard/vad_model/background.gmm"
	#define VAD_ENERGY_INITIAL            0.004
	#define LAMBDA_L                      0.995
	#define LAMBDA_H                      0.993
	#define VAD_THRSHOLD                  500.0
	#else
	#define VAD_SPEECH_MODEL_FILE         "./vad_model/speech.gmm"
	#define VAD_BACKGROUND_MODEL_FILE     "./vad_model/background.gmm"
	#define VAD_ENERGY_INITIAL            0.04
	#define LAMBDA_L                      0.95
	#define LAMBDA_H                      0.93
	#define VAD_THRSHOLD                  0.0
	#endif

	#define SEGMENT_WIN_SIZE              8
	#define VAD_FLAG_TYPE                 0      	// flagtype == 1 show segmental gmm result; flagtype == 0 show single frame gmm result;

#endif

// pre-process
#define PREEMPHASIS_FACTOR 0.97

// windowing
#define WINDOW_TYPE        AUD_WIN_HAMM

// feature extraction

 // MFCC
#define MELFB_LOW          0    // (0.008 * SAMPLE_RATE)
#define MELFB_HIGH         8000 // (0.42 * SAMPLE_RATE)
#define MELFB_ORDER        30

#define COMPRESS_TYPE      AUD_AMPCOMPRESS_LOG

#define LOG_ENERGY         0  // 0 or 1
#define MFCC_ORDER         28


#define RAW_MFCC           MFCC_ORDER
#define RASTA_MFCC         0 // (MFCC_ORDER + LOG_ENERGY)
#define DELTA_MFCC         0 // (MFCC_ORDER + LOG_ENERGY)
#define DDELTA_MFCC        0
#define FDELTA_MFCC        0

#if DDELTA_MFCC
	#define MFCC_DELAY     8
#elif (DELTA_MFCC || RASTA_MFCC)
	#define MFCC_DELAY     4
#else
	#define MFCC_DELAY     0
#endif

 // spectral feature: currently are sub-band renyi entropy and sub-band spectral centroid
#define SPECFEAT_LEN       0

#define MFCC_FEATDIM       (RAW_MFCC + RASTA_MFCC + DELTA_MFCC + DDELTA_MFCC + FDELTA_MFCC + SPECFEAT_LEN)

#define ENABLE_MVN         0  // cepstral mean & variance normalization


/****************************************************************
 * Wake On Voice
 ****************************************************************/
// DTW parameters
#define WOV_DTW_TYPE              AUD_DTWTYPE_SUBSEQUENCE // AUD_DTWTYPE_CLASSIC
#define WOV_DTWTRANSITION_STYLE   AUD_DTWTRANSITION_LEVENSHTEIN
#define WOV_DTWSCORING_METHOD     AUD_DTWSCORE_BEST       // AUD_DTWSCORE_TERMINAL
#define WOV_DISTANCE_METRIC       AUD_DISTTYPE_COSINE

#define WOV_DTWMATCH_FLOOR_RATIO  0.6
#define WOV_DTWMATCH_CEIL_RATIO   1.8

// GMM parameters
 // GMM model files
#if defined( ANDROID )
#define WOV_UBM_GMMMODEL_FILE      "/sdcard/wov/gmm_model/ubm-512.gmm"
#define WOV_IMPOSTER_GMMMODEL_FILE "/sdcard/wov/gmm_model/hellointel-imposter.gmm"
#define WOV_KEYWORD_GMMMODEL_FILE  "/sdcard/wov/gmm_model/hellointel.gmm"
#define WOV_SPEAKER_GMMMODEL_FILE  "/sdcard/wov/gmm_model/hellointel-speaker.gmm"
#define WOV_IMPOSTER_GMMMODEL_DIR  "/sdcard/wov/gmm_model/imposters"
#define WOV_KEYWORD_GMMMODEL_DIR   "/sdcard/wov/gmm_model/keywords"
#else
#define WOV_UBM_GMMMODEL_FILE     "./wov_model/gmm_model/ubm-512.gmm"
#define WOV_IMPOSTER_GMMMODEL_DIR "./wov_model/gmm_model/imposters"
#define WOV_KEYWORD_GMMMODEL_DIR  "./wov_model/gmm_model/keywords"
#endif

// HMM-GMM parameters
  // GMM training parameters
#define WOV_KEYWORD_GMMMODEL_ORDER 16

  // HMM training parameters
#define WOV_GMM_CLUSTER_THRESHOLD  4.5f
#define WOV_HMM_MAP_TAU            20.0f

  // GMM-HMM model files
#if defined( ANDROID )
#define WOV_UBM_GMMHMMMODEL_FILE      "/sdcard/wov/hmm_model/ubm-512.gmm"
#define WOV_KEYWORD_GMMHMMMODEL_DIR   "/sdcard/wov/hmm_model/keywords"
#else
#define WOV_UBM_GMMHMMMODEL_FILE      "./wov_model/hmm_model/ubm-512.gmm"
#define WOV_KEYWORD_GMMHMMMODEL_DIR   "./wov_model/hmm_model/keywords"
#endif


/****************************************************************
 * Speaker Verification
 ****************************************************************/
#if defined ( ANDROID )
#define SV_UBM_GMMMODEL_FILE      "/sdcard/sv/model/ubm-512.gmm"
#define SV_SPEAKER_GMMMODEL_DIR   "/sdcard/sv/model/speakers"
#define SV_UBM_GMMMODEL_NAME      "ubm-512.gmm"
#define SV_UBM_GMMMODEL_DIR       "/sdcard/sv/model"
#else
#define SV_UBM_GMMMODEL_FILE     "./sv_model/ubm-512.gmm"
#define SV_SPEAKER_GMMMODEL_DIR  "./sv_model/speakers"
#endif



/****************************************************************
 * Performance Benchmark
 ****************************************************************/
// log dump mechanism
#define DUMP_DETAIL            1

#endif // _AUDIO_CONFIG_H
